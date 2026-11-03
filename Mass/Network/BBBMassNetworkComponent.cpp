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
    const APlayerController* Controller = Cast<APlayerController>(GetOwner());
    const APlayerState* Player = Controller != nullptr ? Controller->GetPlayerState<APlayerState>() : nullptr;
    Contribution.bWorldSource = false;
    Contribution.PlayerId = Player != nullptr ? Player->GetPlayerId() : INDEX_NONE;
    if (!InstanceId.IsValid() || !Contribution.IsValid())
    {
        return;
    }

    for (TActorIterator<ABBBMassNetworkActor> It(GetWorld()); It; ++It)
    {
        const FMassEntityHandle Entity = It->FindEntity(InstanceId);
        UBBBMassSubsystem* Mass = GetWorld()->GetSubsystem<UBBBMassSubsystem>();
        FBBBMonsterDamageTotalsRemoteMessagePacket Packet;
        Packet.InstanceId = InstanceId;
        Packet.Include(Contribution);
        Mass->SubmitInput(Entity, MoveTemp(Packet));
        return;
    }
}
