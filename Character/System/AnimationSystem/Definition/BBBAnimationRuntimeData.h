
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/Commands/BBBCharacterAnimationCommands.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "BBBAnimationRuntimeData.generated.h"
class FBBBCharacterAimPresentationProcessor;
class FBBBCharacterEquipmentPoseProcessor;
class FBBBCharacterInitializer;

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

    UPROPERTY()
    FBBBCharacterAnimationCommands Commands;
    FBBBAimPresentationRuntimeState AimPresentation;
};
