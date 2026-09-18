#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBCharacterRestoreLocomotionInput.generated.h"

/** 网络还原步态事实 */
USTRUCT()
struct FBBBCharacterRestoreLocomotionInput
{
    GENERATED_BODY()

    EBBBCharacterGait Gait = EBBBCharacterGait::Run;
};
