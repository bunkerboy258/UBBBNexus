
#pragma once
#include "CoreMinimal.h"
class UBBBCharacterNetworkComponent;
struct FBBBCharacterNetworkConfig;
struct FBBBAimRuntimeData;
struct FBBBNetworkRuntimeData;

class ABBB_EVAC_API FBBBAimUploadProcessor final
{
public:

    /**
     * 按节流条件上传本地瞄准状态
     * @param AimData	瞄准运行时数据
     * @param NetworkConfig	网络配置
     * @param WorldTimeSeconds	当前世界时间
     * @param NetworkData	网络运行时数据
     * @param NetworkComponent	角色网络组件
     */
    void Update(
        const FBBBAimRuntimeData &AimData,
        const FBBBCharacterNetworkConfig &NetworkConfig,
        float WorldTimeSeconds,
        FBBBNetworkRuntimeData &NetworkData,
        UBBBCharacterNetworkComponent &NetworkComponent) const;
};
