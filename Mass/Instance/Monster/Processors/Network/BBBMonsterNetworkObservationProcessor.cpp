#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Network/BBBMonsterNetworkObservationProcessor.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "MassExecutionContext.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Network/BBBMassNetworkActor.h"
#include "BBBWork/UBBBNexus/Mass/Network/BBBMassNetworkComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"

UBBBMonsterNetworkObservationProcessor::UBBBMonsterNetworkObservationProcessor()
    : EntityQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    QueryBasedPruning = EMassQueryBasedPruning::Never;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Network;
    ExecutionOrder.ExecuteAfter.Add(BBBMassProcessingGroups::Collision);
}

void UBBBMonsterNetworkObservationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FBBBMonsterDamageFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FBBBMonsterBehaviorFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadWrite);
}

void UBBBMonsterNetworkObservationProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    if (World->GetNetMode() == NM_Standalone)
    {
        return;
    }

    const bool bHost = World->GetNetMode() != NM_Client;
    const bool bUpdateAction = World->GetTimeSeconds() >= NextPublishTime;
    if (bUpdateAction)
    {
        NextPublishTime = World->GetTimeSeconds() + 0.1f;
    }
    ABBBMassNetworkActor* Transport = nullptr;
    for (TActorIterator<ABBBMassNetworkActor> It(World); It; ++It)
    {
        Transport = *It;
        break;
    }
    if (bHost)
    {
        if (Transport == nullptr)
        {
            Transport = World->SpawnActor<ABBBMassNetworkActor>();
        }
        for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
        {
            APlayerController* Controller = It->Get();
            if (Controller != nullptr && Controller->FindComponentByClass<UBBBMassNetworkComponent>() == nullptr)
            {
                UBBBMassNetworkComponent* Component = NewObject<UBBBMassNetworkComponent>(Controller);
                Controller->AddInstanceComponent(Component);
                Component->RegisterComponent();
            }
        }
    }
    if (Transport == nullptr)
    {
        return;
    }

    const APlayerController* LocalController = World->GetFirstPlayerController();
    const APlayerState* LocalPlayer = LocalController != nullptr
        ? LocalController->GetPlayerState<APlayerState>() : nullptr;
    const int32 LocalPlayerId = LocalPlayer != nullptr ? LocalPlayer->GetPlayerId() : INDEX_NONE;
    if (bHost)
    {
        Transport->BeginPublish();
    }

    EntityQuery.ForEachEntityChunk(Context, [Transport, bHost, bUpdateAction, LocalPlayerId](FMassExecutionContext& Chunk)
    {
        const auto Transforms = Chunk.GetFragmentView<FTransformFragment>();
        const auto Velocity = Chunk.GetFragmentView<FMassVelocityFragment>();
        const auto Damage = Chunk.GetFragmentView<FBBBMonsterDamageFragment>();
        const auto Behavior = Chunk.GetFragmentView<FBBBMonsterBehaviorFragment>();
        auto Network = Chunk.GetMutableFragmentView<FBBBMonsterNetworkFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            if (!bHost)
            {
                const double* LocalDamage = Damage[Index].Contributions.Find(LocalPlayerId);
                Network[Index].bDamageSubmitted = Network[Index].InstanceId.IsValid()
                    && Transport->ReportLocalDamage(Network[Index].InstanceId, LocalPlayerId,
                        LocalDamage != nullptr ? *LocalDamage : 0.0);
                continue;
            }

            if (!Network[Index].InstanceId.IsValid())
            {
                Network[Index].InstanceId = FGuid::NewGuid();
            }
            FBBBMonsterReplicationItem Item;
            Item.InstanceId = Network[Index].InstanceId;
            Item.Definition = Network[Index].Definition.Get();
            Item.Location = Transforms[Index].GetTransform().GetLocation();
            Item.Rotation = Transforms[Index].GetTransform().Rotator();
            Item.Velocity = Velocity[Index].Value;
            Item.Behavior = Behavior[Index].State;
            Item.ActionId = Behavior[Index].ActionId;
            Item.StateEnteredTime = Behavior[Index].StateEnteredTime;
            for (const auto& Value : Damage[Index].Contributions)
            {
                Item.Contributions.Add({Value.Key, Value.Value});
            }
            Item.Contributions.Sort([](const auto& A, const auto& B)
            {
                return A.PlayerId < B.PlayerId;
            });
            Transport->Publish(Item, Chunk.GetEntity(Index), bUpdateAction);
            Network[Index].bDamageSubmitted = true;
        }
    });
    if (bHost)
    {
        Transport->EndPublish();
    }
}
