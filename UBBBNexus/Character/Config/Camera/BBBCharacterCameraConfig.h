#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterCameraConfig.generated.h"

/** 角色侧相机构图与回正 不随武器变化 */
USTRUCT(BlueprintType)
struct FBBBCharacterCameraConfig
{
    GENERATED_BODY()

    /** 常态相机臂长度 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "构图", meta = (ClampMin = "1.0", DisplayName = "常态相机臂长度", ForceUnits = "cm"))
    float HipArmLength = 365.0f;

    /** 瞄准相机臂长度 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "构图", meta = (ClampMin = "1.0", DisplayName = "瞄准相机臂长度", ForceUnits = "cm"))
    float AimArmLength = 155.0f;

    /** 常态镜头水平视角 单位度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "构图", meta = (ClampMin = "30.0", ClampMax = "120.0", DisplayName = "常态水平视角", ForceUnits = "deg"))
    float HipFieldOfView = 78.0f;

    /** 瞄准镜头水平视角 单位度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "构图", meta = (ClampMin = "30.0", ClampMax = "120.0", DisplayName = "瞄准水平视角", ForceUnits = "deg"))
    float AimFieldOfView = 62.0f;

    /** 常态镜头向右偏移 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "构图", meta = (ClampMin = "0.0", DisplayName = "常态肩位偏移", ForceUnits = "cm"))
    float HipShoulderOffset = 48.0f;

    /** 瞄准镜头向右偏移 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "构图", meta = (ClampMin = "0.0", DisplayName = "瞄准肩位偏移", ForceUnits = "cm"))
    float AimShoulderOffset = 60.0f;

    /** 常态镜头枢轴相对胶囊中心的高度 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "构图", meta = (DisplayName = "常态枢轴高度", ForceUnits = "cm"))
    float HipPivotHeight = 91.0f;

    /** 站立瞄准的镜头枢轴相对胶囊中心的高度 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "构图", meta = (DisplayName = "瞄准枢轴高度", ForceUnits = "cm"))
    float AimPivotHeight = 58.0f;

    /** 蹲伏瞄准在胶囊中心随姿态下降后额外降低的枢轴高度 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "构图", meta = (ClampMin = "0.0", DisplayName = "蹲伏瞄准额外降幅", ForceUnits = "cm"))
    float AimCrouchPivotDrop = 20.0f;

    /** 举枪或收枪时相机构图完整过渡的时间 单位秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "构图", meta = (ClampMin = "0.01", DisplayName = "举枪收枪过渡时间", ForceUnits = "s"))
    float AimBlendDuration = 0.20f;

    /** 瞄准时从镜头量起的景深对焦距离 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "景深", meta = (ClampMin = "100.0", DisplayName = "瞄准对焦距离", ForceUnits = "cm"))
    float AimFocusDistance = 1800.0f;

    /** 瞄准景深的光圈值 数值越大近景模糊越弱 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "景深", meta = (ClampMin = "1.0", ClampMax = "32.0", DisplayName = "瞄准景深光圈"))
    float AimDepthOfFieldFStop = 2.8f;

    /** 胶囊高度变化的镜头过渡时间 单位秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "构图", meta = (ClampMin = "0.01", DisplayName = "站蹲高度过渡时间", ForceUnits = "s"))
    float StanceHeightBlendDuration = 0.2f;

    /** 停火后可回零偏移的指数回正速度 角色统一 连发窗口内不衰减 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "镜头回正", meta = (ClampMin = "0.01", DisplayName = "镜头回正速度 (1/秒)", ToolTip = "指数衰减系数 仅在距上次开火超过武器开火间隔后生效 越大可回零偏移恢复越快 不恢复已保留的俯仰角 不随武器改变"))
    float RecoverySpeed = 10.0f;
};
