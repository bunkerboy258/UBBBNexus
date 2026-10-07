#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Animation/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/PlayerCamera/Config/BBBPlayerCameraRecoilSettings.h"
#include "BBBRifleAnimInstance.generated.h"

class UBBBRifleDefinition;

/** 步枪动画图只读的动作结果 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBRifleAnimInstance final : public UBBBEquipmentAnimInstance
{
    GENERATED_BODY()

public:
    /** @return 本次武器快照的世界时间 */
    UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
    float GetSnapshotTimeSeconds() const
    {
        return SnapshotTimeSeconds;
    }

    /** @return 当前武器已经成立的开火序号 */
    UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
    int32 GetFireSequence() const
    {
        return FireSequence;
    }

    /** @return 腰射相机后坐力配置快照 */
    UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
    FBBBPlayerCameraRecoilSettings GetHipFireCameraSettings() const
    {
        return HipFireCameraSettings;
    }

    /** @return 瞄准相机后坐力配置快照 */
    UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
    FBBBPlayerCameraRecoilSettings GetAimFireCameraSettings() const
    {
        return AimFireCameraSettings;
    }

    /** @return 空中相机冲量倍率快照 */
    UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
    float GetAirborneCameraImpulseScale() const
    {
        return AirborneCameraImpulseScale;
    }

    /** @return 空中相机回零速度倍率快照 */
    UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
    float GetAirborneCameraRecoveryScale() const
    {
        return AirborneCameraRecoveryScale;
    }

    /** @return 步枪是否正在换弹 */
    UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
    bool IsReloading() const
    {
        return bIsReloading;
    }

    /** @return 最近一次开火至当前快照的间隔 */
    UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
    float GetTimeSinceLastFireSeconds() const
    {
        return TimeSinceLastFireSeconds;
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
