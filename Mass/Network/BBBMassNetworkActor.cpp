#include "BBBWork/UBBBNexus/Mass/Network/BBBMassNetworkActor.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "MassSpawnerSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Network/BBBMassNetworkComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/AuthorityFact/Network/FBBBMonsterStateAuthorityFactPacket.h"

ABBBMassNetworkActor::ABBBMassNetworkActor()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    bAlwaysRelevant = true;
    SetNetUpdateFrequency(10.0f);
}

void ABBBMassNetworkActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ABBBMassNetworkActor, Monsters);
}

void ABBBMassNetworkActor::BeginPublish()
{
    Observed.Reset();
}

void ABBBMassNetworkActor::Publish(const FBBBMonsterReplicationItem& State, FMassEntityHandle Entity, bool bUpdateAction)
{
    Observed.Add(State.InstanceId);
    LocalEntities.Add(State.InstanceId, Entity);
    const int32* ExistingIndex = ReplicationIndices.Find(State.InstanceId);
    FBBBMonsterReplicationItem* Existing = ExistingIndex != nullptr ? &Monsters.Items[*ExistingIndex] : nullptr;
    if (Existing == nullptr)
    {
        Existing = &Monsters.Items.AddDefaulted_GetRef();
        ReplicationIndices.Add(State.InstanceId, Monsters.Items.Num() - 1);
        bUpdateAction = true;
    }

    const bool bDamageChanged = Existing->Contributions != State.Contributions;
    const bool bGoreChanged = FMemory::Memcmp(Existing->DismembermentPresets, State.DismembermentPresets, sizeof(State.DismembermentPresets)) != 0;
    const bool bActionChanged = bUpdateAction && (Existing->InstanceId != State.InstanceId
        || Existing->Definition != State.Definition || !Existing->Location.Equals(State.Location, 0.1f)
        || !Existing->Rotation.Equals(State.Rotation, 0.1f) || !Existing->Velocity.Equals(State.Velocity, 0.1f)
        || Existing->Behavior != State.Behavior || Existing->ActionId != State.ActionId
        || Existing->StateEnteredTime != State.StateEnteredTime || Existing->CrawlStartedAt != State.CrawlStartedAt);
    if (!bDamageChanged && !bActionChanged && !bGoreChanged)
    {
        return;
    }

    Existing->InstanceId = State.InstanceId;
    Existing->BirthSeed = State.BirthSeed;
    FMemory::Memcpy(Existing->DismembermentPresets, State.DismembermentPresets, sizeof(State.DismembermentPresets));
    Existing->Definition = State.Definition;
    if (bUpdateAction)
    {
        Existing->Location = State.Location;
        Existing->Rotation = State.Rotation;
        Existing->Velocity = State.Velocity;
        Existing->Behavior = State.Behavior;
        Existing->ActionId = State.ActionId;
        Existing->StateEnteredTime = State.StateEnteredTime;
        Existing->CrawlStartedAt = State.CrawlStartedAt;
    }
    Existing->Contributions = State.Contributions;
    ++Existing->Revision;
    Monsters.MarkItemDirty(*Existing);

    if (bDamageChanged)
    {
        // 属性快照提供当前结果 RPC 保证实体很快回收时最终贡献也已经进入可靠通道
        MulticastDamage(State.InstanceId, State.Contributions);
    }
}

void ABBBMassNetworkActor::EndPublish()
{
    for (int32 Index = Monsters.Items.Num() - 1; Index >= 0; --Index)
    {
        if (!Observed.Contains(Monsters.Items[Index].InstanceId))
        {
            LocalEntities.Remove(Monsters.Items[Index].InstanceId);
            ReplicationIndices.Remove(Monsters.Items[Index].InstanceId);
            Monsters.Items.RemoveAtSwap(Index);
            if (Monsters.Items.IsValidIndex(Index))
            {
                ReplicationIndices.Add(Monsters.Items[Index].InstanceId, Index);
            }
            Monsters.MarkArrayDirty();
        }
    }
}

FMassEntityHandle ABBBMassNetworkActor::FindEntity(const FGuid& InstanceId) const
{
    const auto* Entity = LocalEntities.Find(InstanceId);
    return Entity != nullptr ? *Entity : FMassEntityHandle();
}

bool ABBBMassNetworkActor::ReportLocalDamage(const FGuid& InstanceId, const FBBBMonsterDamageContribution& Contribution)
{
    const auto* Submitted = SubmittedDamage.Find(InstanceId);
    if (Contribution.Parts.Sum() <= 0.0 || (Submitted != nullptr
        && !Contribution.Parts.Exceeds(Submitted->Parts) && !Contribution.Destruction.Exceeds(Submitted->Destruction)))
    {
        return true;
    }
    APlayerController* Controller = GetWorld()->GetFirstPlayerController();
    const APlayerState* Player = Controller != nullptr ? Controller->GetPlayerState<APlayerState>() : nullptr;
    UBBBMassNetworkComponent* Connection = Controller != nullptr
        ? Controller->FindComponentByClass<UBBBMassNetworkComponent>() : nullptr;
    if (Player == nullptr || Player->GetPlayerId() != Contribution.PlayerId || Connection == nullptr || !Contribution.IsValid())
    {
        return false;
    }
    Connection->ServerReportDamage(InstanceId, Contribution);
    SubmittedDamage.Add(InstanceId, Contribution);
    return true;
}

void ABBBMassNetworkActor::MulticastDamage_Implementation(FGuid InstanceId,
    const TArray<FBBBMonsterDamageContribution>& Contributions)
{
    if (!HasAuthority())
    {
        RouteDamage(InstanceId, Contributions);
    }
}

void ABBBMassNetworkActor::RouteDamage(const FGuid& InstanceId,
    const TArray<FBBBMonsterDamageContribution>& Contributions)
{
    PruneCaches();
    if (Retired.Contains(InstanceId))
    {
        return;
    }
    UBBBMassSubsystem* Mass = GetWorld()->GetSubsystem<UBBBMassSubsystem>();
    FBBBMonsterDamageRemoteMessagePacket Packet;
    Packet.InstanceId = InstanceId;
    const FMassEntityHandle Entity = FindEntity(InstanceId);
    if (!Mass->QueryDamage(Entity, Packet.Contributions))
    {
        /** 完整复制快照仍会还原当前贡献 暂存不能随短命实体无限增长 */
        if (!PendingDamageSnapshots.Contains(InstanceId) && PendingDamageSnapshots.Num() >= 512)
        {
            return;
        }
        auto& Pending = PendingDamageSnapshots.FindOrAdd(InstanceId);
        Pending.Packet.InstanceId = InstanceId;
        Pending.UpdatedAt = GetWorld()->GetTimeSeconds();
        for (const auto& Value : Contributions)
        {
            Pending.Packet.Include(Value);
        }
        return;
    }
    for (const auto& Value : Contributions)
    {
        Packet.Include(Value);
    }
    if (Packet.IsValid())
    {
        Mass->SubmitInput(Entity, MoveTemp(Packet));
    }
}

void ABBBMassNetworkActor::PruneCaches()
{
    const float Now = GetWorld()->GetTimeSeconds();
    if (Now < NextCachePruneTime)
    {
        return;
    }
    NextCachePruneTime = Now + 1.0f;
    TSet<FGuid> Present;
    for (const auto& Item : Monsters.Items)
    {
        Present.Add(Item.InstanceId);
    }
    for (auto It = PendingDamageSnapshots.CreateIterator(); It; ++It)
    {
        if (!Present.Contains(It.Key()) && Now - It.Value().UpdatedAt >= 30.0f)
        {
            It.RemoveCurrent();
        }
    }
    for (auto It = Retired.CreateIterator(); It; ++It)
    {
        if (!Present.Contains(It.Key()) && (Now - It.Value() >= 120.0f || Retired.Num() > 2048))
        {
            It.RemoveCurrent();
        }
    }
}

void ABBBMassNetworkActor::OnRep_Monsters()
{
    PruneCaches();
    UBBBMassSubsystem* Mass = GetWorld()->GetSubsystem<UBBBMassSubsystem>();
    UMassEntitySubsystem* Entities = GetWorld()->GetSubsystem<UMassEntitySubsystem>();
    if (!ensureMsgf(Mass != nullptr && Entities != nullptr, TEXT("Mass 网络还原缺少世界依赖")))
    {
        return;
    }

    FMassEntityManager& Manager = Entities->GetMutableEntityManager();
    TSet<FGuid> Present;
    ReplicationIndices.Reset();
    for (int32 ItemIndex = 0; ItemIndex < Monsters.Items.Num(); ++ItemIndex)
    {
        const auto& Item = Monsters.Items[ItemIndex];
        ReplicationIndices.Add(Item.InstanceId, ItemIndex);
        Present.Add(Item.InstanceId);
        if (Retired.Contains(Item.InstanceId))
        {
            continue;
        }

        FMassEntityHandle Entity = FindEntity(Item.InstanceId);
        if (Entity.IsSet() && !Manager.IsEntityValid(Entity))
        {
            Retired.Add(Item.InstanceId, GetWorld()->GetTimeSeconds());
            PendingDamageSnapshots.Remove(Item.InstanceId);
            continue;
        }

        if (!Entity.IsSet())
        {
            if (!ensureMsgf(Item.Definition != nullptr && Item.Definition->EntityConfig != nullptr,
                TEXT("Mass 远端小怪缺少出生配置")))
            {
                continue;
            }

            Entity = Mass->CreateEntity(*Item.Definition->EntityConfig);
            if (!Entity.IsSet())
            {
                continue;
            }
            LocalEntities.Add(Item.InstanceId, Entity);
        }

        FBBBMonsterStateAuthorityFactPacket Packet;
        Packet.InstanceId = Item.InstanceId;
        Packet.BirthSeed = Item.BirthSeed;
        FMemory::Memcpy(Packet.DismembermentPresets, Item.DismembermentPresets, sizeof(Item.DismembermentPresets));
        Packet.Revision = Item.Revision;
        Packet.Transform = FTransform(Item.Rotation, Item.Location);
        Packet.Velocity = Item.Velocity;
        Packet.Behavior = Item.Behavior;
        Packet.ActionId = Item.ActionId;
        const AGameStateBase* GameState = GetWorld()->GetGameState();
        const double ServerTime = GameState != nullptr ? GameState->GetServerWorldTimeSeconds() : GetWorld()->GetTimeSeconds();
        Packet.StateEnteredTime = GetWorld()->GetTimeSeconds() - FMath::Max(0.0, ServerTime - Item.StateEnteredTime);
        Packet.CrawlStartedAt = Item.CrawlStartedAt >= 0.0f
            ? GetWorld()->GetTimeSeconds() - FMath::Max(0.0, ServerTime - Item.CrawlStartedAt) : -1.0f;
        Packet.bHasCrawlTime = Item.CrawlStartedAt >= 0.0f;
        Mass->SubmitInput(Entity, MoveTemp(Packet));
        RouteDamage(Item.InstanceId, Item.Contributions);
        if (const auto* Pending = PendingDamageSnapshots.Find(Item.InstanceId))
        {
            const auto Contributions = Pending->Packet.Contributions;
            PendingDamageSnapshots.Remove(Item.InstanceId);
            RouteDamage(Item.InstanceId, Contributions);
        }
    }

    for (auto It = LocalEntities.CreateIterator(); It; ++It)
    {
        if (Present.Contains(It.Key()))
        {
            continue;
        }

        Retired.Add(It.Key(), GetWorld()->GetTimeSeconds());
        SubmittedDamage.Remove(It.Key());
        PendingDamageSnapshots.Remove(It.Key());
        if (Manager.IsEntityValid(It.Value()))
        {
            TArray<FMassEntityHandle> Removed;
            Removed.Add(It.Value());
            GetWorld()->GetSubsystem<UMassSpawnerSubsystem>()->DestroyEntities(Removed);
        }
        It.RemoveCurrent();
    }
}
