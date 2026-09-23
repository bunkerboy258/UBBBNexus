#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AimSystem/Processors/BBBCharacterAimStateProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AimSystem/Processors/BBBCharacterAimTargetProcessor.h"

class APawn;
class FBBBCharacterInitializer;
struct FBBBAimAnimationConfig;
struct FBBBAimDomainState;
struct FBBBCharacterEquipmentSelectionState;
struct FBBBCharacterControlState;
struct FBBBCharacterWorldState;

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
     * @param InAimConfig  瞄准配置
     */
    void Initialize(
        FBBBAimDomainState &InAimData,
        const FBBBCharacterControlState &InIntentData,
        const FBBBCharacterEquipmentSelectionState &InEquipmentState,
        const FBBBCharacterWorldState &InWorldState,
        const FBBBAimAnimationConfig &InAimConfig);

    FBBBAimDomainState *AimData = nullptr;
    const FBBBCharacterControlState *ControlData = nullptr;
    const FBBBCharacterEquipmentSelectionState *EquipmentState = nullptr;
    const FBBBCharacterWorldState *WorldState = nullptr;
    const FBBBAimAnimationConfig *AimConfig = nullptr;

    FBBBCharacterAimStateProcessor AimStateProcessor;
    FBBBCharacterAimTargetProcessor AimTargetProcessor;
};
