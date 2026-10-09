#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Animation/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/PlayerCamera/Config/BBBPlayerCameraRecoilSettings.h"
#include "BBBRifleAnimInstance.generated.h"

class UBBBRifleDefinition;
class UAnimSequence;

/** 步枪动画图只读的动作结果 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBRifleAnimInstance final : public UBBBEquipmentAnimInstance
{
    GENERATED_BODY()

public:
    /** @return 本武器专属的角色后坐力序列快照 */
    UAnimSequence *GetRecoilAnimation() const override
    {
        return RecoilAnimation;
    }

    /** @return 本次武器快照的世界时间 */
    float GetSnapshotTimeSeconds() const override
    {
        return SnapshotTimeSeconds;
    }

    /** @return 当前武器已经成立的开火序号 */
    int32 GetFireSequence() const override
    {
        return FireSequence;
    }

    /** @return 腰射相机后坐力配置快照 */
    FBBBPlayerCameraRecoilSettings GetHipFireCameraSettings() const override
    {
        return HipFireCameraSettings;
    }

    /** @return 瞄准相机后坐力配置快照 */
    FBBBPlayerCameraRecoilSettings GetAimFireCameraSettings() const override
    {
        return AimFireCameraSettings;
    }

    /** @return 空中相机冲量倍率快照 */
    float GetAirborneCameraImpulseScale() const override
    {
        return AirborneCameraImpulseScale;
    }

    /** @return 空中相机回零速度倍率快照 */
    float GetAirborneCameraRecoveryScale() const override
    {
        return AirborneCameraRecoveryScale;
    }

    /** @return 步枪是否正在换弹 */
    bool IsReloading() const override
    {
        return bIsReloading;
    }

    /** @return 最近一次开火至当前快照的间隔 */
    float GetTimeSinceLastFireSeconds() const override
    {
        return TimeSinceLastFireSeconds;
    }

    /** @return 腰射瞄准跟随速度快照 */
    float GetHipFireAimFollowSpeed() const override
    {
        return HipFireAimFollowSpeed;
    }

    /** @return 瞄准射击跟随速度快照 */
    float GetAimFireAimFollowSpeed() const override
    {
        return AimFireAimFollowSpeed;
    }

    /** @return 腰射后向后坐力权重快照 */
    float GetHipFireBackwardRecoilAlpha() const override
    {
        return HipFireBackwardRecoilAlpha;
    }

    /** @return 瞄准射击后向后坐力权重快照 */
    float GetAimFireBackwardRecoilAlpha() const override
    {
        return AimFireBackwardRecoilAlpha;
    }

    /** @return 腰射摇摆幅度快照 */
    FVector2D GetHipFireSwayAmplitudeDegrees() const override
    {
        return HipFireSwayAmplitudeDegrees;
    }

    /** @return 瞄准摇摆幅度快照 */
    FVector2D GetAimFireSwayAmplitudeDegrees() const override
    {
        return AimFireSwayAmplitudeDegrees;
    }

    /** @return 腰射摇摆频率快照 */
    FVector2D GetHipFireSwayFrequency() const override
    {
        return HipFireSwayFrequency;
    }

    /** @return 瞄准摇摆频率快照 */
    FVector2D GetAimFireSwayFrequency() const override
    {
        return AimFireSwayFrequency;
    }

    /** @return 空中瞄准跟随倍率快照 */
    float GetAirborneAimFollowScale() const override
    {
        return AirborneAimFollowScale;
    }

    /** @return 空中后向后坐力倍率快照 */
    float GetAirborneBackwardRecoilScale() const override
    {
        return AirborneBackwardRecoilScale;
    }

    /** @return 空中摇摆幅度倍率快照 */
    float GetAirborneSwayAmplitudeScale() const override
    {
        return AirborneSwayAmplitudeScale;
    }

    /** @return 空中摇摆频率倍率快照 */
    float GetAirborneSwayFrequencyScale() const override
    {
        return AirborneSwayFrequencyScale;
    }

protected:
    /** 当前弹量 */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (DisplayName = "已装填弹药"))
    int32 LoadedAmmo = 0;

    /** 当前弹匣容量 */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (DisplayName = "弹药容量"))
    int32 AmmoCapacity = 0;

    /** 当前换弹状态 */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (DisplayName = "正在换弹"))
    bool bIsReloading = false;

    /** 当前快照距离最近开火的时间 */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (DisplayName = "距上次开火时间 秒"))
    float TimeSinceLastFireSeconds = 0.0f;

private:
    friend class FBBBRifleAnimationProcessor;

    /** 由动画系统发布的专属后坐力序列引用 */
    UPROPERTY(Transient)
    TObjectPtr<UAnimSequence> RecoilAnimation = nullptr;

    /**
     * 发布由动画系统整理的步枪事实
     * @param InLoadedAmmo			当前弹量
     * @param InAmmoCapacity			弹匣容量
     * @param bInReloading			当前换弹状态
     * @param InTimeSinceLastFireSeconds	距最近开火的时间
     * @return 无
     */
    void PublishRifleSnapshot(
        int32 InLoadedAmmo,
        int32 InAmmoCapacity,
        bool bInReloading,
        float InTimeSinceLastFireSeconds,
        int32 InFireSequence,
        float InSnapshotTimeSeconds,
        const UBBBRifleDefinition &Definition);


    /** 腰射目标跟随速度 */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = "true", DisplayName = "腰射瞄准跟随速度"))
    float HipFireAimFollowSpeed = 18.0f;

    /** 瞄准目标跟随速度 */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = "true", DisplayName = "瞄准射击跟随速度"))
    float AimFireAimFollowSpeed = 18.0f;

    /** 腰射向后震动强度 */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = "true", DisplayName = "腰射后向后坐力权重"))
    float HipFireBackwardRecoilAlpha = 0.0f;

    /** 瞄准向后震动强度 */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = "true", DisplayName = "瞄准射击后向后坐力权重"))
    float AimFireBackwardRecoilAlpha = 0.0f;

    /** 腰射摇摆幅度 */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = "true", DisplayName = "腰射摇摆幅度 角度"))
    FVector2D HipFireSwayAmplitudeDegrees = FVector2D::ZeroVector;

    /** 瞄准摇摆幅度 */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = "true", DisplayName = "瞄准射击摇摆幅度 角度"))
    FVector2D AimFireSwayAmplitudeDegrees = FVector2D::ZeroVector;

    /** 腰射摇摆频率 */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = "true", DisplayName = "腰射摇摆频率"))
    FVector2D HipFireSwayFrequency = FVector2D::ZeroVector;

    /** 瞄准摇摆频率 */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = "true", DisplayName = "瞄准射击摇摆频率"))
    FVector2D AimFireSwayFrequency = FVector2D::ZeroVector;

    /** 空中跟随倍率 */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = "true", DisplayName = "空中瞄准跟随倍率"))
    float AirborneAimFollowScale = 1.0f;

    /** 空中向后震动倍率 */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = "true", DisplayName = "空中后向后坐力倍率"))
    float AirborneBackwardRecoilScale = 1.0f;

    /** 空中摇摆幅度倍率 */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = "true", DisplayName = "空中摇摆幅度倍率"))
    float AirborneSwayAmplitudeScale = 1.0f;

    /** 空中摇摆频率倍率 */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = "true", DisplayName = "空中摇摆频率倍率"))
    float AirborneSwayFrequencyScale = 1.0f;

    /** 世界时间基本事实 供动画图计算持枪摇摆相位 */
    float SnapshotTimeSeconds = 0.0f;

    /** 开火事实序号 */
    UPROPERTY(Transient)
    int32 FireSequence = 0;

    /** 腰射镜头配置的只读副本 */
    UPROPERTY(Transient)
    FBBBPlayerCameraRecoilSettings HipFireCameraSettings;

    /** 瞄准镜头配置的只读副本 */
    UPROPERTY(Transient)
    FBBBPlayerCameraRecoilSettings AimFireCameraSettings;

    /** 空中镜头冲量倍率 */
    float AirborneCameraImpulseScale = 1.0f;

    /** 空中镜头回零倍率 */
    float AirborneCameraRecoveryScale = 1.0f;
};
