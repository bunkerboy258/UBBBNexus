#pragma once

#include "CoreMinimal.h"

struct FBBBCharacterNetworkUpdateContext;

/** 观察并上传当前主手装备状态 */
class ABBB_EVAC_API FBBBEquipmentStateObservationProcessor final
{
public:
    /**
     * 当前主手装备变化时上传装备状态
     * @param Context 本次网络更新上下文
     * @return 无
     */
    void Update(FBBBCharacterNetworkUpdateContext &Context) const;
};
