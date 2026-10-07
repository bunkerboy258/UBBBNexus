#include "BBBWork/UBBBNexus/Mass/Network/BBBMassNetworkComponent.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "BBBWork/UBBBNexus/Mass/Network/BBBMassNetworkActor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/RemoteMessage/Health/FBBBMonsterDamageRemoteMessagePacket.h"

UBBBMassNetworkComponent::UBBBMassNetworkComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UBBBMassNetworkComponent::ServerReportDamage_Implementation(FGuid InstanceId, FBBBMonsterDamageContribution Contribution)
{
    const APlayerController* Controller = Cast<APlayerController>(GetOwner());
    const APlayerState* Player = Controller != nullptr ? Controller->GetPlayerState<APlayerState>() : nullptr;
    Contribution.PlayerId = Player != nullptr ? Player->GetPlayerId() : INDEX_NONE;
    if (!InstanceId.IsValid() || !Contribution.IsValid())
    {
        return;
    }

    for (TActorIterator<ABBBMassNetworkActor> It(GetWorld()); It; ++It)
    {
        const FMassEntityHandle Entity = It->FindEntity(InstanceId);
        UBBBMassSubsystem* Mass = GetWorld()->GetSubsystem<UBBBMassSubsystem>();
        FBBBMonsterDamageRemoteMessagePacket Packet;
        Packet.InstanceId = InstanceId;
        // 查询包含本轮其他连接已投递的结果 再生成新的完整覆盖包
        if (Mass->QueryDamage(Entity, Packet.Contributions))
        {
            Packet.Include(Contribution);
            Mass->SubmitInput(Entity, MoveTemp(Packet));
        }
        return;
    }
}
