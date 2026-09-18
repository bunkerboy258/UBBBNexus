#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterControlInput.generated.h"

/** 外部控制源提交的世界空间控制快照 */
USTRUCT(BlueprintType)
struct FBBBCharacterControlInput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector MoveWorld = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator FacingWorld = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector AimTargetWorld = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bAim = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bFire = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bWalk = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bSprint = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCrouch = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bJump = false;
};

/** 控制快照唯一的跨字段裁决规则 */
struct FBBBCharacterControlRule
{
    /**
     * 裁决冲刺是否与本帧瞄准和开火冲突
     * @param bAiming	是否瞄准
     * @param bFiring	是否开火
     * @return 是否允许冲刺
     */
    static bool AllowsSprint(bool bAiming, bool bFiring);
};
