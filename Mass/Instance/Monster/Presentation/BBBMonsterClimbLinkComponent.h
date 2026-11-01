#pragma once

#include "NavLinkCustomComponent.h"
#include "BBBMonsterClimbLinkComponent.generated.h"

/** 在寻路阶段按当前身体能力过滤每条台沿连接 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterClimbLinkComponent final : public UNavLinkCustomComponent
{
    GENERATED_BODY()

public:
    /**
     * @param Querier	当前同步导航参数载体
     * @return 入口处于允许高度且当前可用
     */
    virtual bool IsLinkPathfindingAllowed(const UObject* Querier) const override;
};
