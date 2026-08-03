
#pragma once
#include "CoreMinimal.h"
class UBBBCharacterNetworkComponent;
struct FBBBAimConfig;
struct FBBBAimRuntimeData;
struct FBBBNetworkRuntimeData;

class ABBB_EVAC_API FBBBAimUploadProcessor final
{
public:

    /**
     * 按节流条件上传本地瞄准状态
     * @param AimData	瞄准运行时数据
     * @param AimConfig	瞄准配置
     * @param WorldTimeSeconds	当前世界时间
     * @param NetworkData	网络运行时数据
     * @param NetworkComponent	角色网络组件
     */
    void Update(
        const FBBBAimRuntimeData &AimData,
        const FBBBAimConfig &AimConfig,
        float WorldTimeSeconds,
        FBBBNetworkRuntimeData &NetworkData,
        UBBBCharacterNetworkComponent &NetworkComponent) const;
};
