#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitRegion.h"
#include "BBBMonsterHitShapeDefinition.generated.h"

/** 目标自身的逻辑命中球或骨骼段 */
USTRUCT(BlueprintType)
struct FBBBMonsterHitShapeDefinition final
{
    GENERATED_BODY()

    /** 本形状所属生命部位 */
    UPROPERTY(EditAnywhere, meta = (DisplayName = "所属部位"))
    EBBBMonsterHitRegion Region = EBBBMonsterHitRegion::Torso;

    /** 起点骨骼 */
    UPROPERTY(EditAnywhere, meta = (DisplayName = "起点骨骼"))
    FName Bone = NAME_None;

    /** 终点骨骼 为空表示单球 */
    UPROPERTY(EditAnywhere, meta = (DisplayName = "终点骨骼"))
    FName EndBone = NAME_None;

    /** 无可用身体时相对逻辑中心的起点 */
    UPROPERTY(EditAnywhere, meta = (DisplayName = "备用起点", Units = "cm"))
    FVector FallbackStart = FVector::ZeroVector;

    /** 无可用身体时相对逻辑中心的终点 */
    UPROPERTY(EditAnywhere, meta = (DisplayName = "备用终点", Units = "cm"))
    FVector FallbackEnd = FVector::ZeroVector;

    /** 单球相对起点骨骼的局部偏移 */
    UPROPERTY(EditAnywhere, meta = (DisplayName = "骨骼局部偏移", Units = "cm"))
    FVector Offset = FVector::ZeroVector;

    /** 球或骨骼段的覆盖半径 */
    UPROPERTY(EditAnywhere, meta = (DisplayName = "命中半径", ClampMin = "0.1", Units = "cm"))
    float Radius = 10.0f;

    /** @return 目标形状参数是否合法 */
    bool IsValid() const
    {
        return Region <= EBBBMonsterHitRegion::RightLeg && !Bone.IsNone()
            && !FallbackStart.ContainsNaN() && !FallbackEnd.ContainsNaN() && !Offset.ContainsNaN()
            && FMath::IsFinite(Radius) && Radius > 0;
    }
};
