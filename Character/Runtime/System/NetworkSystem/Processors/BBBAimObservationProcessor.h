
#pragma once
#include "CoreMinimal.h"
class FBBBCharacterNetworkSystem;
struct FBBBCharacterNetworkConfig;
struct FBBBAimRuntimeData;
struct FBBBNetworkState;

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
    void Update(
        const FBBBAimRuntimeData &AimData,
        const FBBBCharacterNetworkConfig &NetworkConfig,
        float WorldTimeSeconds,
        FBBBNetworkState &NetworkData,
        FBBBCharacterNetworkSystem &NetworkSystem) const;
};
