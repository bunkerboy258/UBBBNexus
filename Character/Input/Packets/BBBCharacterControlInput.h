#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterControlInput.generated.h"

/** 外部控制源提交的世界空间控制事实 */
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
