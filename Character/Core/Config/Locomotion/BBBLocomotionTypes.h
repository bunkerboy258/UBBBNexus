
#pragma once
#include "CoreMinimal.h"
#include "BBBLocomotionTypes.generated.h"

USTRUCT(BlueprintType)
//定义自由移动与持械移动的速度加速度和平滑
struct FBBBCharacterLocomotionConfig
{
    GENERATED_BODY()

    //设置自由慢速移动的最大速度
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Walk")
    float WalkSpeed = 280.0f;

    //设置自由慢速移动达到目标速度的加速度
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Walk")
    float WalkAcceleration = 800.0f;

    //设置自由快速移动的最大速度
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Run")
    float RunSpeed = 650.0f;

    //设置自由快速移动达到目标速度的加速度
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Run")
    float RunAcceleration = 1600.0f;

    //设置持械慢速横移的最大速度
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|StrafeWalk")
    float StrafeWalkSpeed = 280.0f;

    //设置持械慢速横移达到目标速度的加速度
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|StrafeWalk")
    float StrafeWalkAcceleration = 800.0f;

    //设置持械快速横移的最大速度
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|StrafeRun")
    float StrafeRunSpeed = 360.0f;

    //设置持械快速横移达到目标速度的加速度
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|StrafeRun")
    float StrafeRunAcceleration = 1200.0f;

    //定义横向输入从旧值过渡到新值的时间
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Smoothing", meta = (ClampMin = "0.001"))
    float MoveInputXSmoothTime = 0.1f;

    //定义纵向输入从旧值过渡到新值的时间
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Locomotion|Smoothing", meta = (ClampMin = "0.001"))
    float MoveInputYSmoothTime = 0.1f;
};
