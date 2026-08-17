
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/Commands/BBBCharacterAnimationCommands.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "BBBAnimationRuntimeData.generated.h"
class FBBBCharacterAimPresentationProcessor;
class FBBBCharacterEquipmentPoseProcessor;
class FBBBCharacterInitializer;
class FBBBCharacterLocomotionFactsProcessor;

USTRUCT(BlueprintType)
//角色运行时动画数据
struct FBBBAnimationRuntimeData
{
    GENERATED_BODY()

    /**
     * 读取动画命令缓冲的第一级封装
     * @return 动画命令容器
     */
    const FBBBCharacterAnimationCommands &GetCommands() const
    {
        return Commands;
    }

    /**
     * 清理本帧动画命令
     */
    void CleanFrame()
    {
        Commands.CleanFrame();
    }
private:
    friend class FBBBCharacterAimPresentationProcessor;
    friend class FBBBCharacterEquipmentPoseProcessor;
    friend class FBBBCharacterInitializer;
    friend class FBBBCharacterLocomotionFactsProcessor;

    /** 动画命令缓冲 */
    UPROPERTY()
    FBBBCharacterAnimationCommands Commands;
    /** 瞄准表现处理器的跨帧平滑状态 */
    FBBBAimPresentationRuntimeState AimPresentation;

    /** 实际转向速度计算使用的跨帧状态 */
    FBBBCharacterTurnTrackingState TurnTracking;

    /** 上一次移动状态解算使用的跨帧状态 */
    FBBBCharacterLocomotionTrackingState LocomotionTracking;
};

