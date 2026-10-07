#pragma once

#include "Components/ActorComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageContribution.h"
#include "BBBMassNetworkComponent.generated.h"

/** 挂在连接所有者上的客机累计贡献上报通道 */
UCLASS()
class ABBB_EVAC_API UBBBMassNetworkComponent final : public UActorComponent
{
    GENERATED_BODY()

public:
    UBBBMassNetworkComponent();

    /** 来源由连接所属 PlayerState 确定 只传这一代小怪的最新累计值 */
    UFUNCTION(Server, Reliable)
    void ServerReportDamage(FGuid InstanceId, FBBBMonsterDamageContribution Contribution);
};
