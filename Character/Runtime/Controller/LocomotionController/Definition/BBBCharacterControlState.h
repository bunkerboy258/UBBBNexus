#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterControlState.generated.h"

/** 解析后的角色控制黑板 */
USTRUCT()
struct FBBBCharacterControlFacts
{
    GENERATED_BODY()

    FVector MoveWorld = FVector::ZeroVector;
    FRotator FacingWorld = FRotator::ZeroRotator;
    FVector AimTargetWorld = FVector::ZeroVector;
    bool bAim = false;
    bool bFire = false;
    bool bWalk = false;
    bool bSprint = false;
    bool bCrouch = false;
    bool bJump = false;
};

/** 已裁决的角色控制黑板 */
USTRUCT()
struct FBBBCharacterControlState
{
    GENERATED_BODY()

    UPROPERTY()
    FBBBCharacterControlFacts Value;
};
