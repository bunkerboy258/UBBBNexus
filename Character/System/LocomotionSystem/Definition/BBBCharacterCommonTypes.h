
#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterCommonTypes.generated.h"

UENUM(BlueprintType)
enum class EBBBLocomotionState : uint8
{
    Idle UMETA(DisplayName = "Idle"),

    Walk UMETA(DisplayName = "Walk"),

    Run UMETA(DisplayName = "Run"),

    StrafeWalk UMETA(DisplayName = "StrafeWalk"),

    StrafeRun UMETA(DisplayName = "StrafeRun")
};
