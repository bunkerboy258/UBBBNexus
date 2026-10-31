#pragma once

#include "BBBMonsterGibPartPresentation.h"
#include "BBBMonsterFleshPresentation.h"
#include "BBBMonsterGibPresentation.generated.h"

/** 残肢运动和生命周期 世界级预算不在此扩大 */
USTRUCT(BlueprintType)
struct FBBBMonsterGibPresentation final
{
    GENERATED_BODY()

    FBBBMonsterGibPresentation();

    UPROPERTY(EditAnywhere, meta = (DisplayName = "命中碎肉"))
    FBBBMonsterFleshPresentation Flesh;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "默认残肢"))
    FBBBMonsterGibPartPresentation DefaultPart;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "骨骼残肢差异", TitleProperty = "Bone"))
    TArray<FBBBMonsterGibPartPresentation> Parts;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "飞行速度随机范围"))
    FVector2D SpeedScale = FVector2D(0.9, 1.1);

    UPROPERTY(EditAnywhere, meta = (DisplayName = "横向散射速度", ClampMin = "0", ClampMax = "300"))
    float SideSpeed = 45;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "旋转方向偏差", ClampMin = "0", ClampMax = "1"))
    float SpinDirectionSpread = 0.35f;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "继承身体速度比例", ClampMin = "0", ClampMax = "2"))
    float InheritedVelocityScale = 1;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "继承速度上限", ClampMin = "0", ClampMax = "1500", Units = "cm/s"))
    float MaximumInheritedSpeed = 650;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "保留秒数", ClampMin = "1", ClampMax = "120"))
    float Lifetime = 30;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "淡出秒数", ClampMin = "0.1", ClampMax = "10"))
    float FadeDuration = 5;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "休眠线速度阈值", ClampMin = "1", ClampMax = "30"))
    float RestLinearSpeed = 15;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "休眠角速度阈值", ClampMin = "1", ClampMax = "30"))
    float RestAngularSpeed = 15;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "落地稳定等待秒数", ClampMin = "0.1", ClampMax = "2"))
    float RestDuration = 0.5f;

    /** @param Bone 断开骨骼 @return 已制作的对应参数或明确默认参数 */
    const FBBBMonsterGibPartPresentation& ReadPart(FName Bone) const;
};
