#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/Definition/States/BBBAimStates.h"
#include "BBBCharacterRestoreAimInput.generated.h"

/** 网络还原瞄准事实 */
USTRUCT()
struct FBBBCharacterRestoreAimInput
{
    GENERATED_BODY()

    FBBBAimRuntimeState State;
};
