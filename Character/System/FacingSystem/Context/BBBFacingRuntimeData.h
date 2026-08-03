
#pragma once
#include "CoreMinimal.h"
#include "BBBFacingRuntimeData.generated.h"

USTRUCT(BlueprintType)
struct FBBBFacingRuntimeData
{
    GENERATED_BODY()

    /**
     * 角色身体是否正在转向
     * @return 身体正在转向时返回true
     */
    bool IsBodyTurning() const
    {
        return bIsBodyTurning;
    }

    /**
     * 读取当前水平瞄准角绝对值
     * @return 水平瞄准角绝对值
     */
    float GetCurrentAimYawAbs() const
    {
        return CurrentAimYawAbs;
    }

    /**
     * 读取上一帧是否面向瞄准方向
     * @return 上一帧面向瞄准方向时返回true
     */
    bool GetLastFaceAimDirection() const
    {
        return bLastFaceAimDirection;
    }

    /**
     * 是否已应用过旋转模式
     * @return 已应用过旋转模式时返回true
     */
    bool HasAppliedRotationMode() const
    {
        return bHasAppliedRotationMode;
    }

    /**
     * 提交朝向状态
     * @param bInIsBodyTurning	身体是否正在转向
     * @param InCurrentAimYawAbs	当前水平瞄准角绝对值
     * @param bInLastFaceAimDirection	上一帧是否面向瞄准方向
     * @param bInHasAppliedRotationMode	是否已应用过旋转模式
     */
    void CommitState(
        bool bInIsBodyTurning,
        float InCurrentAimYawAbs,
        bool bInLastFaceAimDirection,
        bool bInHasAppliedRotationMode)
    {
        bIsBodyTurning = bInIsBodyTurning;
        CurrentAimYawAbs = InCurrentAimYawAbs;
        bLastFaceAimDirection = bInLastFaceAimDirection;
        bHasAppliedRotationMode = bInHasAppliedRotationMode;
    }
private:

    UPROPERTY()
    bool bIsBodyTurning = false;

    UPROPERTY()
    float CurrentAimYawAbs = 0.0f;

    bool bLastFaceAimDirection = false;

    bool bHasAppliedRotationMode = false;
};
