
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBCharacterAnimationCommands.h"
#include "BBBAnimationRuntimeData.generated.h"
class FBBBCharacterAimPresentationProcessor;
class FBBBCharacterEquipmentPoseProcessor;
class FBBBCharacterInitializer;

//瞄准表现处理器的跨帧平滑工作状态
struct FBBBAimPresentationRuntimeState
{
    //保存上一帧平滑后的组件空间瞄准目标
    FVector SmoothedAimTargetComponentSpace = FVector::ZeroVector;

    //保存平滑算法的速度状态
    FVector AimTargetSmoothVelocity = FVector::ZeroVector;

    //保存跨帧插值累积的瞄准IK权重
    float SmoothedAimIKAlpha = 0.0f;

    //标记是否已有可用的平滑目标历史值
    bool bHasSmoothedAimTarget = false;
};

USTRUCT(BlueprintType)
//角色运行时动画数据
struct FBBBAnimationRuntimeData
{
    GENERATED_BODY()

    //读取动画命令缓冲的第一级封装
    const FBBBCharacterAnimationCommands &GetCommands() const
    {
        return Commands;
    }

    void CleanFrame()
    {
        Commands.CleanFrame();
    }
private:
    friend class FBBBCharacterAimPresentationProcessor;
    friend class FBBBCharacterEquipmentPoseProcessor;
    friend class FBBBCharacterInitializer;

    UPROPERTY()
    FBBBCharacterAnimationCommands Commands;
    FBBBAimPresentationRuntimeState AimPresentation;
};
