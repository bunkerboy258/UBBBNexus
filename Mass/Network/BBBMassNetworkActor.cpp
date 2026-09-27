#include "BBBWork/UBBBNexus/Mass/Network/BBBMassNetworkActor.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
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

void ABBBMassNetworkActor::Publish(const FBBBMonsterReplicationItem& State, FMassEntityHandle Entity)
{
    Observed.Add(State.InstanceId);
    LocalEntities.Add(State.InstanceId, Entity);
    const int32* ExistingIndex = ReplicationIndices.Find(State.InstanceId);
    FBBBMonsterReplicationItem* Existing = ExistingIndex != nullptr ? &Monsters.Items[*ExistingIndex] : nullptr;

    if (Existing == nullptr)
    {
        Existing = &Monsters.Items.AddDefaulted_GetRef();
        ReplicationIndices.Add(State.InstanceId, Monsters.Items.Num() - 1);
    }

    // 只标记实际发生变化的当前结果 不发送逐帧历史
    if (Existing->InstanceId == State.InstanceId && Existing->Definition == State.Definition
        && Existing->Location.Equals(State.Location, 0.1f) && Existing->Rotation.Equals(State.Rotation, 0.1f)
        && Existing->Velocity.Equals(State.Velocity, 0.1f) && Existing->Behavior == State.Behavior
        && Existing->ActionId == State.ActionId && Existing->StateEnteredTime == State.StateEnteredTime
        && Existing->Health == State.Health)
    {
        return;
    }

    Existing->InstanceId = State.InstanceId;
    Existing->Definition = State.Definition;
    Existing->Location = State.Location;
    Existing->Rotation = State.Rotation;
    Existing->Velocity = State.Velocity;
    Existing->Behavior = State.Behavior;
    Existing->ActionId = State.ActionId;
    Existing->StateEnteredTime = State.StateEnteredTime;
    Existing->Health = State.Health;
    ++Existing->Revision;
    Monsters.MarkItemDirty(*Existing);
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

void ABBBMassNetworkActor::ObserveLocalHealth(const FGuid& InstanceId, float Health)
{
    LocalHealth.Add(InstanceId, Health);
}

void ABBBMassNetworkActor::SendLocalHealth()
{
    APlayerController* Controller = GetWorld()->GetFirstPlayerController();
    UBBBMassNetworkComponent* Connection = Controller != nullptr ? Controller->FindComponentByClass<UBBBMassNetworkComponent>() : nullptr;
    if (Connection == nullptr)
    {
        return;
    }

    for (auto It = LocalHealth.CreateIterator(); It; ++It)
    {
        const int32* Index = ReplicationIndices.Find(It.Key());
        const auto* Server = Index != nullptr ? &Monsters.Items[*Index] : nullptr;
        if (Server == nullptr || Server->Health <= It.Value())
        {
            It.RemoveCurrent();
            continue;
        }

        // 未获回显的当前结果继续提交 不是逐次扣血事件重放
        Connection->ServerReportHealth(It.Key(), It.Value());
    }
}

void ABBBMassNetworkActor::OnRep_Monsters()
{
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
            // 本机已经回收的死亡实例保留身份墓碑 不被迟到存活包重新生成
            Retired.Add(Item.InstanceId);
            continue;
        }

        if (!Entity.IsSet())
        {
            if (Item.Health <= 0.0f)
            {
                Retired.Add(Item.InstanceId);
                continue;
            }

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
        Packet.Revision = Item.Revision;
        Packet.Transform = FTransform(Item.Rotation, Item.Location);
        Packet.Velocity = Item.Velocity;
        Packet.Behavior = Item.Behavior;
        Packet.ActionId = Item.ActionId;
        Packet.Health = Item.Health;
        const AGameStateBase* GameState = GetWorld()->GetGameState();
        const double ServerTime = GameState != nullptr ? GameState->GetServerWorldTimeSeconds() : GetWorld()->GetTimeSeconds();
        Packet.StateEnteredTime = GetWorld()->GetTimeSeconds() - FMath::Max(0.0, ServerTime - Item.StateEnteredTime);
        Mass->SubmitInput(Entity, MoveTemp(Packet));
    }

    for (auto It = LocalEntities.CreateIterator(); It; ++It)
    {
        if (Present.Contains(It.Key()))
        {
            continue;
        }

        Retired.Add(It.Key());
        LocalHealth.Remove(It.Key());
        if (Manager.IsEntityValid(It.Value()))
        {
            TArray<FMassEntityHandle> Removed;
            Removed.Add(It.Value());
            GetWorld()->GetSubsystem<UMassSpawnerSubsystem>()->DestroyEntities(Removed);
        }
        It.RemoveCurrent();
    }
}
