#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterControlFacts.generated.h"

/** 已裁决的角色控制事实 */
USTRUCT()
struct FBBBCharacterControlFacts
{
    GENERATED_BODY()

    UPROPERTY()
    FVector MoveWorld = FVector::ZeroVector;

    UPROPERTY()
    FRotator FacingWorld = FRotator::ZeroRotator;

    UPROPERTY()
    FVector AimTargetWorld = FVector::ZeroVector;

    UPROPERTY()
    bool bAim = false;

    UPROPERTY()
    bool bFire = false;

    UPROPERTY()
    bool bWalk = false;

    UPROPERTY()
    bool bSprint = false;

    UPROPERTY()
    bool bCrouch = false;

    UPROPERTY()
    bool bJump = false;
};
