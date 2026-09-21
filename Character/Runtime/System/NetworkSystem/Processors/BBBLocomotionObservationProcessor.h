#pragma once

#include "CoreMinimal.h"

struct FBBBCharacterNetworkUpdateContext;

/** 将本地移动步态变化上传到权威端 */
class ABBB_EVAC_API FBBBLocomotionObservationProcessor final
{
public:
    /**
     * 上传发生变化的移动步态
     * @param LocomotionData 移动运行时数据
     * @param NetworkData 网络运行时数据
     * @param NetworkSystem 角色网络系统
     */
    /**
     * 上传发生变化的移动步态
     * @param Context 本次网络更新上下文
     * @return 无
     */
    void Update(FBBBCharacterNetworkUpdateContext &Context) const;
};
