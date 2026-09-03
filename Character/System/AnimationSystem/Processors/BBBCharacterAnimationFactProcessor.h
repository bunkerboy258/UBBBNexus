#pragma once

#include "CoreMinimal.h"

class ABBBCharacter;
struct FBBBCharacterAnimationFacts;
struct FBBBCharacterRuntimeData;

/** 将角色运行状态转换为动画蓝图可安全读取的事实快照 */
class ABBB_EVAC_API FBBBCharacterAnimationFactProcessor final
{
public:
    /**
     * 采集移动完成后的角色事实
     * @param Character		角色
     * @param RuntimeData	角色运行时数据
     * @param OutFacts		输出事实快照
     * @param DeltaSeconds	本帧间隔
     * @return 无
     */
    void Update(
        ABBBCharacter &Character,
        FBBBCharacterRuntimeData &RuntimeData,
        FBBBCharacterAnimationFacts &OutFacts,
        float DeltaSeconds);

private:
    /**
     * 对组件空间瞄准目标执行无回弹临界平滑
     * @param Current		当前目标
     * @param Target		原始目标
     * @param Velocity		平滑速度状态
     * @param SmoothTime	平滑时间
     * @param DeltaSeconds	本帧间隔
     * @return 平滑后的目标
     */
    FVector SmoothAimTarget(
        const FVector &Current,
        const FVector &Target,
        FVector &Velocity,
        float SmoothTime,
        float DeltaSeconds) const;

    FVector AimTargetSmoothVelocity = FVector::ZeroVector;

    FVector SmoothedAimTargetComponentSpace = FVector::ZeroVector;

    float SmoothedAimIntentAlpha = 0.0f;

    float SmoothedAimIKLockAlpha = 1.0f;

    bool bHasSmoothedAimTarget = false;
};
