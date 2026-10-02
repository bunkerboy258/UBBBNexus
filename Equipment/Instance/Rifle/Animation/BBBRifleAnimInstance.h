#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Animation/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Config/BBBRifleDefinition.h"
#include "BBBRifleAnimInstance.generated.h"

/** 步枪动画图只读的动作结果 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBRifleAnimInstance : public UBBBEquipmentAnimInstance
{
    GENERATED_BODY()

public:
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

    /** @return 腰射持枪参数快照 */
    UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
    FBBBRifleHandlingSettings GetHipFireSettings() const
    {
        return HipFireSettings;
    }

    /** @return 瞄准持枪参数快照 */
    UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
    FBBBRifleHandlingSettings GetAimFireSettings() const
    {
        return AimFireSettings;
    }

    /** @return 空中持枪修正快照 */
    UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
    FBBBRifleAirborneModifiers GetAirborneModifiers() const
    {
        return AirborneModifiers;
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
    UPROPERTY(BlueprintReadOnly, Transient)
    int32 LoadedAmmo = 0;

    /** 当前弹匣容量 */
    UPROPERTY(BlueprintReadOnly, Transient)
    int32 AmmoCapacity = 0;

    /** 当前换弹状态 */
    UPROPERTY(BlueprintReadOnly, Transient)
    bool bIsReloading = false;

    /** 当前快照距离最近开火的时间 */
    UPROPERTY(BlueprintReadOnly, Transient)
    float TimeSinceLastFireSeconds = 0.0f;

private:
    /** 世界时间基本事实 供动画图计算持枪摇摆相位 */
    float SnapshotTimeSeconds = 0.0f;

    /** 开火事实序号 */
    UPROPERTY(Transient)
    int32 FireSequence = 0;

    /** 腰射静态参数的只读副本 */
    UPROPERTY(Transient)
    FBBBRifleHandlingSettings HipFireSettings;

    /** 瞄准静态参数的只读副本 */
    UPROPERTY(Transient)
    FBBBRifleHandlingSettings AimFireSettings;

    /** 空中修正的只读副本 */
    UPROPERTY(Transient)
    FBBBRifleAirborneModifiers AirborneModifiers;

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
