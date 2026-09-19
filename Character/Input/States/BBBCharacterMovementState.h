#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterMovementState.generated.h"

struct FBBBCharacterControlFacts;

/** 移动和朝向持续状态 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBCharacterMovementState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector MoveWorld = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator FacingWorld = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bWalk = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bSprint = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCrouch = false;

    /** @return 状态内容是否合法 */
    bool IsValid() const
    {
        return !MoveWorld.ContainsNaN() && !FacingWorld.ContainsNaN();
    }

    /** 将移动向量钳制到单位长度 保留模拟量强度 */
    void Sanitize()
    {
        MoveWorld = MoveWorld.GetClampedToMaxSize(1.0f);
    }

    /**
     * 将状态写入控制事实基座
     * @param Control	待写入的控制事实
     */
    void Apply(FBBBCharacterControlFacts &Control) const;
};
