#include "BBBMonsterDestructionValidationLibrary.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterGoreFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Health/FBBBMonsterDamageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"
#include "MassActorSubsystem.h"
#include "MassEntitySubsystem.h"
#include "MassEntityQuery.h"
#include "MassExecutionContext.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Components/SkeletalMeshComponent.h"
#include "Serialization/JsonSerializer.h"

int32 UBBBMonsterDestructionValidationLibrary::SetPopulationDestruction(UObject* WorldContext,
    const TArray<FMassEntityHandle>& Entities, const int32 RegionCount, const bool bSeverLeftArm, const float TorsoDamage)
{
    auto* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull) : nullptr;
    if (!IsInGameThread() || !World || World->WorldType != EWorldType::PIE || Entities.IsEmpty()
        || Entities.Num() > 1000 || RegionCount < 0 || RegionCount > 8 || !FMath::IsFinite(TorsoDamage)
        || TorsoDamage < 0 || TorsoDamage > 10000)
    {
        return 0;
    }
    auto* Mass = World->GetSubsystem<UBBBMassSubsystem>();
    auto* Subsystem = World->GetSubsystem<UMassEntitySubsystem>();
    auto* Controller = World->GetFirstPlayerController();
    auto* Player = Controller ? Controller->GetPlayerState<APlayerState>() : nullptr;
    if (!Mass || !Subsystem || !Player || Player->GetPlayerId() < 0)
    {
        return 0;
    }
    auto& Manager = Subsystem->GetMutableEntityManager();
    int32 Submitted = 0;
    for (const auto Entity : Entities)
    {
        if (!Manager.IsEntityValid(Entity))
        {
            continue;
        }
        const auto* Damage = Manager.GetFragmentDataPtr<FBBBMonsterDamageFragment>(Entity);
        const auto* Network = Manager.GetFragmentDataPtr<FBBBMonsterNetworkFragment>(Entity);
        if (!Damage || !Network || !Network->Definition.IsValid())
        {
            continue;
        }
        const auto* Existing = Damage->Contributions.Find(Player->GetPlayerId());
        auto Contribution = Existing ? *Existing : FBBBMonsterDamageContribution();
        Contribution.PlayerId = Player->GetPlayerId();
        Contribution.Parts.Torso += FMath::Max(TorsoDamage, RegionCount * 0.01f);
        for (int32 Index = 0; Index < RegionCount; ++Index)
        {
            Contribution.Destruction.Regions[Index] = 255;
        }
        if (bSeverLeftArm)
        {
            const auto& Definition = *Network->Definition.Get();
            Contribution.Parts.LeftArm = FMath::Max(Contribution.Parts.LeftArm,
                static_cast<double>(Definition.MaxHealth * Definition.LeftArmPart.HealthFraction + 1.0f));
        }
        Contribution.LastHitTime = World->GetTimeSeconds();
        FBBBMonsterDamageLocalControlPacket Packet;
        Packet.Include(Contribution);
        Submitted += Mass->SubmitInput(Entity, MoveTemp(Packet)) ? 1 : 0;
    }
    return Submitted;
}

FString UBBBMonsterDestructionValidationLibrary::InspectPopulationDestruction(UObject* WorldContext,
    const TArray<FMassEntityHandle>& Entities)
{
    auto* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull) : nullptr;
    auto* Subsystem = World ? World->GetSubsystem<UMassEntitySubsystem>() : nullptr;
    if (!IsInGameThread() || !World || World->WorldType != EWorldType::PIE || !Subsystem)
    {
        return TEXT("{\"error\":\"需要当前PIE世界\"}");
    }
    auto& Manager = Subsystem->GetMutableEntityManager();
    TArray<FMassEntityHandle> Selected = Entities;
    if (Selected.IsEmpty())
    {
        FMassEntityQuery Query(Manager.AsShared());
        Query.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
        Query.AddRequirement<FBBBMonsterGoreFragment>(EMassFragmentAccess::ReadOnly);
        FMassExecutionContext Context(Manager, 0);
        Query.ForEachEntityChunk(Context, [&Selected](FMassExecutionContext& Chunk)
        {
            Selected.Append(Chunk.GetEntities());
        });
    }
    TArray<TSharedPtr<FJsonValue>> Rows;
    for (const auto Entity : Selected)
    {
        if (!Manager.IsEntityValid(Entity))
        {
            continue;
        }
        const auto* Gore = Manager.GetFragmentDataPtr<FBBBMonsterGoreFragment>(Entity);
        const auto* Network = Manager.GetFragmentDataPtr<FBBBMonsterNetworkFragment>(Entity);
        if (!Gore || !Network)
        {
            continue;
        }
        auto Row = MakeShared<FJsonObject>();
        Row->SetStringField(TEXT("instanceId"), Network->InstanceId.ToString());
        TArray<TSharedPtr<FJsonValue>> Regions;
        for (const uint8 Strength : Gore->DestructionStrengths)
        {
            Regions.Add(MakeShared<FJsonValueNumber>(Strength));
        }
        Row->SetArrayField(TEXT("regions"), Regions);
        TArray<TSharedPtr<FJsonValue>> Parts;
        for (const uint8 Preset : Gore->DismembermentPresets)
        {
            Parts.Add(MakeShared<FJsonValueNumber>(Preset));
        }
        Row->SetArrayField(TEXT("dismemberment"), Parts);
        const auto* Fragment = Manager.GetFragmentDataPtr<FMassActorFragment>(Entity);
        const auto* Actor = Fragment ? Fragment->Get() : nullptr;
        TArray<TSharedPtr<FJsonValue>> Meshes;
        if (Actor)
        {
            Row->SetStringField(TEXT("actor"), Actor->GetPathName());
            TArray<USkeletalMeshComponent*> Components;
            Actor->GetComponents(Components);
            for (const auto* Component : Components)
            {
                auto Mesh = MakeShared<FJsonObject>();
                Mesh->SetStringField(TEXT("component"), Component->GetPathName());
                Mesh->SetBoolField(TEXT("visible"), Component->IsVisible());
                Mesh->SetBoolField(TEXT("follower"), Component->LeaderPoseComponent.IsValid());
                Mesh->SetNumberField(TEXT("lod"), Component->GetPredictedLODLevel());
                Meshes.Add(MakeShared<FJsonValueObject>(Mesh));
            }
        }
        Row->SetArrayField(TEXT("meshes"), Meshes);
        Rows.Add(MakeShared<FJsonValueObject>(Row));
    }
    auto Report = MakeShared<FJsonObject>();
    Report->SetStringField(TEXT("world"), World->GetPathName());
    Report->SetArrayField(TEXT("entities"), Rows);
    FString Result;
    FJsonSerializer::Serialize(Report, TJsonWriterFactory<>::Create(&Result));
    return Result;
}
