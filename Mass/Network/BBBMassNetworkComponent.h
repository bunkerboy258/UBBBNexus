#pragma once

#include "Components/ActorComponent.h"
#include "BBBMassNetworkComponent.generated.h"

/** 挂在连接所属控制器上的 Mass 客机上报通道 */
UCLASS()
class ABBB_EVAC_API UBBBMassNetworkComponent final : public UActorComponent
{
    GENERATED_BODY()

public:
    UBBBMassNetworkComponent();

    /**
     * @param InstanceId	目标小怪身份
     * @param Health	本机已经成立的剩余血量
     * @return 无
     */
    UFUNCTION(Server, Reliable)
    void ServerReportHealth(FGuid InstanceId, float Health);
};
