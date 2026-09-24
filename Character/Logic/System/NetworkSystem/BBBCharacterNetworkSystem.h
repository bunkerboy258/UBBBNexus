#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/Processors/BBBAimObservationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/Processors/BBBRunObservationProcessor.h"

class FBBBCharacterInitializer;
class UBBBCharacterNetworkComponent;
struct FBBBCharacterNetworkConfig;
struct FBBBCharacterRuntimeData;

/** 角色网络观察与传输的唯一系统根 */
class ABBB_EVAC_API FBBBCharacterNetworkSystem final
{
public:
    /** 观察已经成立的角色事实并按本机身份发送 */
    void Update();

private:
    friend class FBBBCharacterInitializer;

    /**
     * 注入角色网络系统依赖
     * @param InRuntimeData 角色完整运行时黑板
     * @param InNetworkComponent 角色网络传输组件
     * @param InNetworkConfig 网络发送配置
     * @return 无
     */
    void Initialize(
        FBBBCharacterRuntimeData &InRuntimeData,
        UBBBCharacterNetworkComponent &InNetworkComponent,
        const FBBBCharacterNetworkConfig &InNetworkConfig);

    /** 角色完整运行时黑板 */
    FBBBCharacterRuntimeData *RuntimeData = nullptr;

    /** 角色网络传输组件 */
    UBBBCharacterNetworkComponent *NetworkComponent = nullptr;

    /** 角色网络发送配置 */
    const FBBBCharacterNetworkConfig *NetworkConfig = nullptr;

    FBBBAimObservationProcessor AimObservationProcessor;
    FBBBRunObservationProcessor RunObservationProcessor;
};
