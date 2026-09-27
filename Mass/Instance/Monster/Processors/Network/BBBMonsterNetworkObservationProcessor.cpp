#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Network/BBBMonsterNetworkObservationProcessor.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "MassExecutionContext.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Network/BBBMassNetworkActor.h"
#include "BBBWork/UBBBNexus/Mass/Network/BBBMassNetworkComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
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
    EntityQuery.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FBBBMonsterBehaviorFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadWrite);
}

void UBBBMonsterNetworkObservationProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    if (World->GetNetMode() == NM_Standalone || World->GetTimeSeconds() < NextPublishTime)
    {
        return;
    }

    NextPublishTime = World->GetTimeSeconds() + 0.1f;
    ABBBMassNetworkActor* Transport = nullptr;
    for (TActorIterator<ABBBMassNetworkActor> It(World); It; ++It)
    {
        Transport = *It;
        break;
    }

    const bool bHost = World->GetNetMode() != NM_Client;
    if (bHost)
    {
        if (Transport == nullptr)
        {
            Transport = World->SpawnActor<ABBBMassNetworkActor>();
        }

        // 动态复制组件挂在连接所有者上 客机不向无所有权的小怪 Actor 发 RPC
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

    if (bHost)
    {
        Transport->BeginPublish();
    }

    EntityQuery.ForEachEntityChunk(Context, [Transport, bHost](FMassExecutionContext& Chunk)
    {
        const auto Transforms = Chunk.GetFragmentView<FTransformFragment>();
        const auto Velocity = Chunk.GetFragmentView<FMassVelocityFragment>();
        const auto Health = Chunk.GetFragmentView<FBBBMonsterHealthFragment>();
        const auto Behavior = Chunk.GetFragmentView<FBBBMonsterBehaviorFragment>();
        auto Network = Chunk.GetMutableFragmentView<FBBBMonsterNetworkFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            if (!bHost)
            {
                if (Network[Index].InstanceId.IsValid())
                {
                    Transport->ObserveLocalHealth(Network[Index].InstanceId, Health[Index].CurrentHealth);
                }
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
            Item.Health = Health[Index].CurrentHealth;
            Transport->Publish(Item, Chunk.GetEntity(Index));
        }
    });

    if (bHost)
    {
        Transport->EndPublish();
    }

    if (!bHost)
    {
        Transport->SendLocalHealth();
    }
}
