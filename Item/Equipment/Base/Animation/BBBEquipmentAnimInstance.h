#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Curves/CurveFloat.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Animation/BBBEquipmentAnimationFacts.h"
#include "BBBWork/UBBBNexus/Player/Camera/Config/BBBPlayerCameraRecoilSettings.h"
#include "BBBEquipmentAnimInstance.generated.h"

class UAnimMontage;
class UAnimSequence;

/** 只读装备动画事实快照的动画实例 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBEquipmentAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    /**
     * 在装备动画实例上播放已经批准的蒙太奇
     * @param Montage 待播放的装备蒙太奇
     * @return 是否成功开始播放
     */
    bool PlayEquipmentMontage(UAnimMontage &Montage);

    /** @return 本帧装备动画事实 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    FBBBEquipmentAnimationFacts GetAnimationFacts() const
    {
        return AnimationFacts;
    }

    /** @return 左手握持目标在角色 hand_r 骨骼空间中的位置 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    FVector GetLeftHandTargetHandRSpace() const
    {
        return AnimationFacts.LeftHandTargetHandRSpace;
    }

    /** @return 左手握持目标相对角色 hand_r 骨骼的旋转 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    FRotator GetLeftHandTargetHandRSpaceRotation() const
    {
        return AnimationFacts.LeftHandTargetHandRSpaceRotation;
    }

    /** @return 本帧左手握持目标是否有效 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    bool HasLeftHandTarget() const
    {
        return AnimationFacts.bHasLeftHandTarget;
    }

    /** @return 当前装备的角色后坐力序列 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    virtual UAnimSequence *GetRecoilAnimation() const
    {
        return nullptr;
    }

    /** @return 当前装备动作快照的时间 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    virtual float GetSnapshotTimeSeconds() const
    {
        return 0.0f;
    }

    /** @return 当前装备成立的开火序号 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    virtual int32 GetFireSequence() const
    {
        return 0;
    }

    /** @return 当前装备是否正在换弹 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    virtual bool IsReloading() const
    {
        return false;
    }

    /** @return 当前快照距离最近开火的秒数 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    virtual float GetTimeSinceLastFireSeconds() const
    {
        return 0.0f;
    }

    /** @return 腰射瞄准跟随速度 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    virtual float GetHipFireAimFollowSpeed() const
    {
        return 18.0f;
    }

    /** @return 瞄准射击跟随速度 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    virtual float GetAimFireAimFollowSpeed() const
    {
        return 18.0f;
    }

    /** @return 腰射后向后坐力权重 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    virtual float GetHipFireBackwardRecoilAlpha() const
    {
        return 0.0f;
    }

    /** @return 瞄准射击后向后坐力权重 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    virtual float GetAimFireBackwardRecoilAlpha() const
    {
        return 0.0f;
    }

    /** @return 腰射摇摆幅度 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    virtual FVector2D GetHipFireSwayAmplitudeDegrees() const
    {
        return FVector2D::ZeroVector;
    }

    /** @return 瞄准摇摆幅度 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    virtual FVector2D GetAimFireSwayAmplitudeDegrees() const
    {
        return FVector2D::ZeroVector;
    }

    /** @return 腰射摇摆频率 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    virtual FVector2D GetHipFireSwayFrequency() const
    {
        return FVector2D::ZeroVector;
    }

    /** @return 瞄准摇摆频率 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    virtual FVector2D GetAimFireSwayFrequency() const
    {
        return FVector2D::ZeroVector;
    }

    /** @return 空中瞄准跟随倍率 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    virtual float GetAirborneAimFollowScale() const
    {
        return 1.0f;
    }

    /** @return 空中后向后坐力倍率 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    virtual float GetAirborneBackwardRecoilScale() const
    {
        return 1.0f;
    }

    /** @return 空中摇摆幅度倍率 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    virtual float GetAirborneSwayAmplitudeScale() const
    {
        return 1.0f;
    }

    /** @return 空中摇摆频率倍率 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    virtual float GetAirborneSwayFrequencyScale() const
    {
        return 1.0f;
    }

    /** @return 当前装备腰射镜头贡献 无贡献时冲量为零 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    virtual FBBBPlayerCameraRecoilSettings GetHipFireCameraSettings() const
    {
        FBBBPlayerCameraRecoilSettings Settings;
        Settings.ImpulseDegrees = FVector::ZeroVector;
        Settings.RandomDegrees = FVector::ZeroVector;
        return Settings;
    }

    /** @return 当前装备瞄准镜头贡献 无贡献时冲量为零 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    virtual FBBBPlayerCameraRecoilSettings GetAimFireCameraSettings() const
    {
        FBBBPlayerCameraRecoilSettings Settings;
        Settings.ImpulseDegrees = FVector::ZeroVector;
        Settings.RandomDegrees = FVector::ZeroVector;
        return Settings;
    }

    /** @return 当前装备空中镜头冲量倍率 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    virtual float GetAirborneCameraImpulseScale() const
    {
        return 1.0f;
    }

    /** @return 当前装备空中镜头回零倍率 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe))
    virtual float GetAirborneCameraRecoveryScale() const
    {
        return 1.0f;
    }

    /** @return 下蹲目标跟随倍率 无贡献时为 1 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe, DisplayName = "下蹲目标跟随倍率"))
    virtual float GetCrouchedAimFollowScale() const
    {
        return 1.0f;
    }

    /** @return 下蹲后向震动倍率 无贡献时为 1 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe, DisplayName = "下蹲后向震动倍率"))
    virtual float GetCrouchedBackwardRecoilScale() const
    {
        return 1.0f;
    }

    /** @return 下蹲摇摆幅度倍率 无贡献时为 1 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe, DisplayName = "下蹲摇摆幅度倍率"))
    virtual float GetCrouchedSwayAmplitudeScale() const
    {
        return 1.0f;
    }

    /** @return 下蹲摇摆频率倍率 无贡献时为 1 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe, DisplayName = "下蹲摇摆频率倍率"))
    virtual float GetCrouchedSwayFrequencyScale() const
    {
        return 1.0f;
    }

    /** @return 下蹲镜头冲击倍率 无贡献时为 1 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe, DisplayName = "下蹲镜头冲击倍率"))
    virtual float GetCrouchedCameraImpulseScale() const
    {
        return 1.0f;
    }

    /** @return 下蹲镜头回零速度倍率 无贡献时为 1 */
    UFUNCTION(BlueprintPure, Category = "BBB|装备|动画", meta = (BlueprintThreadSafe, DisplayName = "下蹲镜头回零速度倍率"))
    virtual float GetCrouchedCameraRecoveryScale() const
    {
        return 1.0f;
    }

    /** @return 游戏线程只读的垂直后坐力曲线 */
    const FRichCurve &GetRecoilPitchCurve() const
    {
        return RecoilPitchCurve;
    }

    /** @return 游戏线程只读的水平后坐力曲线 */
    const FRichCurve &GetRecoilYawCurve() const
    {
        return RecoilYawCurve;
    }

    /** @return 连续两枪的配置间隔 单位秒 */
    float GetRecoilFireInterval() const
    {
        return RecoilFireInterval;
    }

    /** @return 停火回落角速度 单位度每秒 */
    float GetRecoilReturnSpeed() const
    {
        return RecoilReturnSpeed;
    }

    /** @return 腰射轨迹幅度倍率 */
    float GetHipRecoilScale() const
    {
        return HipRecoilScale;
    }

    /** @return 瞄准轨迹幅度倍率 */
    float GetAimRecoilScale() const
    {
        return AimRecoilScale;
    }

    /** @return 下蹲轨迹幅度倍率 */
    float GetCrouchedRecoilScale() const
    {
        return CrouchedRecoilScale;
    }

    /** @return 空中轨迹幅度倍率 */
    float GetAirborneRecoilScale() const
    {
        return AirborneRecoilScale;
    }

    /** @return 是否在持续输入期间保持轨迹点 */
    bool HoldsRecoilBetweenShots() const
    {
        return bAutomaticRecoil;
    }

    /** @return 已成立的动作状态是否允许继续连续射击 */
    bool CanSustainRecoil() const
    {
        return bCanSustainRecoil;
    }

private:
    friend class FBBBRifleAnimationProcessor;
    friend class FBBBMeleeAnimationProcessor;
    friend class FBBBPistolAnimationProcessor;
    friend class FBBBRevolverAnimationProcessor;
    friend class FBBBShotgunAnimationProcessor;
    friend class FBBBSMGAnimationProcessor;
    friend class FBBBSniperAnimationProcessor;
    friend class FBBBLMGAnimationProcessor;
    friend class FBBBMinigunAnimationProcessor;

    /**
     * 发布轨迹配置与基本射击许可快照
     * @param PitchCurve 垂直角度曲线
     * @param YawCurve 水平角度曲线
     * @param FireInterval 两枪间隔 单位秒
     * @param ReturnSpeed 回落角速度
     * @param HipScale 腰射倍率
     * @param AimScale 瞄准倍率
     * @param CrouchedScale 下蹲倍率
     * @param AirborneScale 空中倍率
     * @param bAutomatic 是否连续射击
     * @param bCanSustain 当前射击许可
     * @param bRefresh 装备时刷新配置副本
     * @return 无
     */
    void PublishRecoilFacts(const FRuntimeFloatCurve &PitchCurve, const FRuntimeFloatCurve &YawCurve,
        float FireInterval, float ReturnSpeed,
        float HipScale, float AimScale, float CrouchedScale, float AirborneScale,
        bool bAutomatic, bool bCanSustain, bool bRefresh)
    {
        if (bRefresh || !bRecoilProfileInitialized)
        {
            RecoilPitchCurve = *PitchCurve.GetRichCurveConst();
            RecoilYawCurve = *YawCurve.GetRichCurveConst();
            bRecoilProfileInitialized = true;
        }
        RecoilFireInterval = FireInterval;
        RecoilReturnSpeed = ReturnSpeed;
        HipRecoilScale = HipScale;
        AimRecoilScale = AimScale;
        CrouchedRecoilScale = CrouchedScale;
        AirborneRecoilScale = AirborneScale;
        bAutomaticRecoil = bAutomatic;
        bCanSustainRecoil = bCanSustain;
    }

    /** 游戏线程消费的垂直轨迹副本 */
    FRichCurve RecoilPitchCurve;
    /** 游戏线程消费的水平轨迹副本 */
    FRichCurve RecoilYawCurve;
    /** 连续两枪的配置间隔 单位秒 */
    float RecoilFireInterval = 0.2f;
    /** 停火回落角速度 */
    float RecoilReturnSpeed = 12.0f;
    /** 当前姿态幅度倍率 */
    float HipRecoilScale = 1.0f;
    float AimRecoilScale = 1.0f;
    float CrouchedRecoilScale = 1.0f;
    float AirborneRecoilScale = 1.0f;
    /** 射击与配置基本事实 */
    bool bAutomaticRecoil = false;
    bool bCanSustainRecoil = false;
    bool bRecoilProfileInitialized = false;

    /**
     * 一次性发布本帧装备动画事实
     * @param Facts	装备动画系统计算完成的事实
     * @return 无
     */
    void PublishAnimationFacts(const FBBBEquipmentAnimationFacts &Facts);


    UPROPERTY(Transient, BlueprintReadOnly, Category = "BBB|装备|动画", meta = (AllowPrivateAccess = "true", DisplayName = "动画事实"))
    FBBBEquipmentAnimationFacts AnimationFacts;
};
