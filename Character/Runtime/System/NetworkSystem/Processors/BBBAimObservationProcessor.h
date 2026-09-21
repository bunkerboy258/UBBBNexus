
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterNetworkUpdateContext;

class ABBB_EVAC_API FBBBAimObservationProcessor final
{
public:

    /**
     * 按节流条件上传本地瞄准状态
     * @param AimData	瞄准运行时数据
     * @param NetworkConfig	网络配置
     * @param WorldTimeSeconds	当前世界时间
     * @param NetworkData	网络运行时数据
     * @param NetworkSystem	角色网络系统
     */
    /**
     * 按节流条件上传瞄准状态
     * @param Context 本次网络更新上下文
     * @return 无
     */
    void Update(FBBBCharacterNetworkUpdateContext &Context) const;
};
