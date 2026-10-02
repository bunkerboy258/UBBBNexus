#pragma once

#include "CoreMinimal.h"
#include "BBBPlayerCameraRecoilSettings.generated.h"

/** 相机独立消费的三轴开火冲击配置 */
USTRUCT(BlueprintType)
struct FBBBPlayerCameraRecoilSettings
{
    GENERATED_BODY()

    /** 每次开火的上下左右与倾斜冲击 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "相机后坐力", meta = (DisplayName = "镜头冲量 角度", ToolTip = "X 为抬头 Y 为向右 Z 为倾斜 单位为度 不改变角色独立枪口冲量"))
    FVector ImpulseDegrees = FVector(0.6f, 0.0f, 0.12f);

    /** 三轴冲击的对称随机范围 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "相机后坐力", meta = (DisplayName = "镜头随机范围", ToolTip = "各轴在负范围到正范围内随机 单位为度"))
    FVector RandomDegrees = FVector(0.08f, 0.12f, 0.05f);

    /** 相机三轴偏移的指数恢复速度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "相机后坐力", meta = (ClampMin = "0.01", DisplayName = "镜头回零速度", ToolTip = "由相机维护 数值越大镜头冲击越快恢复 与枪口目标跟随速度无关"))
    float RecoverySpeed = 10.0f;

    /** 三轴累计冲击允许的最大绝对角度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "相机后坐力", meta = (DisplayName = "镜头偏移上限", ToolTip = "X 上下 Y 左右 Z 倾斜 单位为度 每个分量必须大于零"))
    FVector LimitDegrees = FVector(5.0f, 3.0f, 2.0f);
};
