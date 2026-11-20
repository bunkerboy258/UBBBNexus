#include "BBBWork/UBBBNexus/Mass/Network/BBBMassNetworkComponent.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "BBBWork/UBBBNexus/Mass/Network/BBBMassNetworkActor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/RemoteMessage/Health/FBBBMonsterDamageTotalsRemoteMessagePacket.h"

UBBBMassNetworkComponent::UBBBMassNetworkComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UBBBMassNetworkComponent::ServerReportDamage_Implementation(FGuid InstanceId, FBBBMonsterDamageTotals Contribution)
{
    TArray<FGuid> InstanceIds;
    InstanceIds.Add(InstanceId);
    TArray<FBBBMonsterDamageTotals> Batch;
    Batch.Add(Contribution);
    ServerReportDamageBatch_Implementation(InstanceIds, Batch);
}

void UBBBMassNetworkComponent::ServerReportDamageBatch_Implementation(const TArray<FGuid>& InstanceIds,
    const TArray<FBBBMonsterDamageTotals>& Contributions)
{
    if (InstanceIds.Num() != Contributions.Num())
    {
        return;
    }
    const APlayerController* Controller = Cast<APlayerController>(GetOwner());
    const APlayerState* Player = Controller != nullptr ? Controller->GetPlayerState<APlayerState>() : nullptr;
    const int32 PlayerId = Player != nullptr ? Player->GetPlayerId() : INDEX_NONE;
    UBBBMassSubsystem* Mass = GetWorld()->GetSubsystem<UBBBMassSubsystem>();
    if (Mass == nullptr)
    {
        return;
    }

    ABBBMassNetworkActor* Transport = nullptr;
    for (TActorIterator<ABBBMassNetworkActor> It(GetWorld()); It; ++It)
    {
        Transport = *It;
        break;
    }
    if (Transport == nullptr)
    {
        return;
    }

    for (int32 Index = 0; Index < InstanceIds.Num(); ++Index)
    {
        FBBBMonsterDamageTotals Contribution = Contributions[Index];
        Contribution.bWorldSource = false;
        Contribution.PlayerId = PlayerId;
        if (!InstanceIds[Index].IsValid() || !Contribution.IsValid())
        {
            continue;
        }
        const FMassEntityHandle Entity = Transport->FindEntity(InstanceIds[Index]);
        FBBBMonsterDamageTotalsRemoteMessagePacket Packet;
        Packet.InstanceId = InstanceIds[Index];
        Packet.Include(Contribution);
        Mass->SubmitInput(Entity, MoveTemp(Packet));
    }
}
