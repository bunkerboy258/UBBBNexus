#include "BBBWork/UBBBNexus/Mass/Network/BBBMassNetworkComponent.h"

#include "EngineUtils.h"
#include "BBBWork/UBBBNexus/Mass/Network/BBBMassNetworkActor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/RemoteMessage/Health/FBBBMonsterHealthRemoteMessagePacket.h"

UBBBMassNetworkComponent::UBBBMassNetworkComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UBBBMassNetworkComponent::ServerReportHealth_Implementation(FGuid InstanceId, float Health)
{
    FBBBMonsterHealthRemoteMessagePacket Packet;
    Packet.Health = Health;
    if (!InstanceId.IsValid() || !Packet.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Mass 收到无效生命结果"));
        return;
    }

    for (TActorIterator<ABBBMassNetworkActor> It(GetWorld()); It; ++It)
    {
        const FMassEntityHandle Entity = It->FindEntity(InstanceId);
        if (Entity.IsSet())
        {
            GetWorld()->GetSubsystem<UBBBMassSubsystem>()->SubmitInput(Entity, MoveTemp(Packet));
        }
        return;
    }
}
