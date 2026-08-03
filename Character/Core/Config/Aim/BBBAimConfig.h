
#pragma once
#include "CoreMinimal.h"
#include "BBBAimConfig.generated.h"

USTRUCT(BlueprintType)
//定义瞄准射线距离与骨骼起点
struct FBBBAimConfig
{
    GENERATED_BODY()

    //限制相机视线检测的最大距离
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimTraceDistance = 10000.0f;

    //指定角色骨骼上的瞄准方向计算起点
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName AimOriginSocketName = FName("spine_03");

    //限制瞄准状态上传的最小时间间隔
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimUploadInterval = 0.033f;
};

USTRUCT(BlueprintType)
//定义瞄准角与逆向动力学表现参数
struct FBBBAimAnimationConfig
{
    GENERATED_BODY()

    //补偿骨骼空间水平旋转幅度
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimYawCompensation = 1.2f;

    //控制瞄准角追随目标的插值速度
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimInterpSpeed = 15.0f;

    //决定是否平滑世界空间逆向动力学目标
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bEnableAimIKTargetSmoothing = true;

    //定义逆向动力学目标平滑到新位置的时间
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimIKTargetSmoothTime = 0.1f;

    //决定近距离目标是否衰减逆向动力学权重
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bEnableNearAimIKDistanceAlpha = true;

    //低于该距离时完全关闭瞄准逆向动力学
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MinAimIKTargetDistance = 60.0f;

    //达到该距离时恢复完整瞄准逆向动力学权重
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float SafeAimIKTargetDistance = 120.0f;

    //控制逆向动力学权重追随目标值的速度
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimIKAlphaInterpSpeed = 8.0f;
};
