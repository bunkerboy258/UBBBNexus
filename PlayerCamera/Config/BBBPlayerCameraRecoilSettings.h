#pragma once

#include "CoreMinimal.h"
#include "BBBPlayerCameraRecoilSettings.generated.h"

/** 相机独立消费的三轴开火冲击配置 */
USTRUCT(BlueprintType)
struct FBBBPlayerCameraRecoilSettings
{
    GENERATED_BODY()

    /** 每次开火的上下左右与倾斜冲击 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "镜头冲击", meta = (DisplayName = "每枪镜头冲击 (度)", ToolTip = "X 抬头 Y 向右 Z 沿视线倾斜 负值反向 绝对值越大冲击越强 与角色枪口方向偏移独立叠加"))
    FVector ImpulseDegrees = FVector(0.6f, 0.0f, 0.12f);

    /** 三轴冲击的对称随机范围 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "镜头冲击", meta = (DisplayName = "镜头冲击随机范围 (度)", ToolTip = "X 上下 Y 左右 Z 倾斜 各轴在负范围至正范围内随机 越大波动越大 0 无随机"))
    FVector RandomDegrees = FVector(0.08f, 0.12f, 0.05f);

    /** 相机三轴偏移的指数恢复速度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "镜头恢复", meta = (ClampMin = "0.01", DisplayName = "镜头回零速率 (1/秒)", ToolTip = "指数衰减系数 越大可回零的镜头偏移恢复越快 不恢复已保留的俯仰角"))
    float RecoverySpeed = 10.0f;

    /** 每枪纵向镜头冲量中不自动回零的比例 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "镜头恢复", meta = (ClampMin = "0.0", ClampMax = "1.0", DisplayName = "俯仰保留比例", ToolTip = "无单位 0 全部自动回零 1 全部留在控制视角中需手动压回 越大保留越多 只作用于 X"))
    float PitchRetention = 0.0f;

    /** 三轴累计冲击允许的最大绝对角度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "镜头限制", meta = (DisplayName = "可回零偏移上限 (度)", ToolTip = "X 上下 Y 左右 Z 倾斜 各轴最大绝对累计偏移 必须大于 0 越大允许偏移越大 不限制已保留的俯仰角"))
    FVector LimitDegrees = FVector(5.0f, 3.0f, 2.0f);
};
