#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Processors/BBBCharacterAimFacingProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Processors/BBBCharacterAimStateProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Processors/BBBCharacterAimTargetProcessor.h"

class APawn;
class FBBBCharacterInitializer;
class UCharacterMovementComponent;
struct FBBBAimConfig;
struct FBBBAimRuntimeData;
struct FBBBCharacterWorldRuntimeData;
struct FBBBIntentRuntimeData;

/**
 * 按角色瞄准状态分流各个瞄准处理器
 */
class ABBB_EVAC_API FBBBCharacterAimSystem final
{
public:
    /**
     * 按固定顺序更新瞄准状态、目标与身体朝向
     */
    void Update();

private:
    friend class FBBBCharacterInitializer;

    /**
     * 初始化瞄准系统依赖
     * @param InPawn       所属角色
     * @param InMovement   角色移动组件
     * @param InAimData    瞄准运行数据
     * @param InIntentData 角色意图数据
     * @param InWorldData  世界帧数据
     * @param InAimConfig  瞄准配置
     */
    void Initialize(
        APawn &InPawn,
        UCharacterMovementComponent &InMovement,
        FBBBAimRuntimeData &InAimData,
        const FBBBIntentRuntimeData &InIntentData,
        const FBBBCharacterWorldRuntimeData &InWorldData,
        const FBBBAimConfig &InAimConfig);

    APawn *Pawn = nullptr;
    UCharacterMovementComponent *Movement = nullptr;
    FBBBAimRuntimeData *AimData = nullptr;
    const FBBBIntentRuntimeData *IntentData = nullptr;
    const FBBBCharacterWorldRuntimeData *WorldData = nullptr;
    const FBBBAimConfig *AimConfig = nullptr;

    FBBBCharacterAimStateProcessor AimStateProcessor;
    FBBBCharacterAimTargetProcessor AimTargetProcessor;
    FBBBCharacterAimFacingProcessor AimFacingProcessor;
};
