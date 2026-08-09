
#pragma once
#include "CoreMinimal.h"
class USkeletalMeshComponent;
struct FBBBAimAnimationConfig;
struct FBBBAimRuntimeData;
struct FBBBAnimationRuntimeData;
struct FBBBCharacterAnimationState;

//把瞄准逻辑转换为动画IK数据
class FBBBCharacterAimPresentationProcessor final
{
public:

    /**
     * 把瞄准逻辑转换为动画IK数据 平滑瞄准目标并提交瞄准表现状态
     * @param CharacterMesh	角色骨骼网格组件
     * @param DeltaSeconds	距上一帧的时间间隔
     * @param AnimationConfig	瞄准动画配置
     * @param AimData	瞄准运行时数据
     * @param AnimationData	动画运行时数据
     * @param AnimationState	输出的动画状态
     */
    void Update(
        USkeletalMeshComponent &CharacterMesh,
        float DeltaSeconds,
        const FBBBAimAnimationConfig &AnimationConfig,
        const FBBBAimRuntimeData &AimData,
        FBBBAnimationRuntimeData &AnimationData,
        FBBBCharacterAnimationState &AnimationState) const;
private:

    /**
     * 带速度项的平滑阻尼跟随目标 防止过冲
     * @param Current	当前值
     * @param Target	目标值
     * @param Velocity	平滑算法的速度状态
     * @param SmoothTime	平滑时间
     * @param DeltaSeconds	距上一帧的时间间隔
     * @return 平滑后的值
     */
    FVector SmoothTarget(
        const FVector &Current,
        const FVector &Target,
        FVector &Velocity,
        float SmoothTime,
        float DeltaSeconds) const;
};
