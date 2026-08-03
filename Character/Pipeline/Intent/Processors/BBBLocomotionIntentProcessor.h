
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterIntentState;
struct FBBBCharacterLocomotionConfig;
struct FBBBInputRuntimeData;
struct FBBBIntentRuntimeData;

class ABBB_EVAC_API FBBBLocomotionIntentProcessor final
{
public:

    /**
     * 计算平滑移动方向与冲刺意图
     * @param InputData	输入后处理数据
     * @param IntentData	上一帧意图运行时数据
     * @param Config	角色移动配置
     * @param DeltaSeconds	帧间隔秒数
     * @param Intent	当前帧意图中间状态
     */
    void Update(
        const FBBBInputRuntimeData &InputData,
        const FBBBIntentRuntimeData &IntentData,
        const FBBBCharacterLocomotionConfig &Config,
        float DeltaSeconds,
        FBBBCharacterIntentState &Intent) const;
};
