#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterRestoreFireInput.generated.h"

/** 网络还原开火事实 */
USTRUCT()
struct FBBBCharacterRestoreFireInput
{
    GENERATED_BODY()

    UPROPERTY()
    FName EquipmentId = NAME_None;

    UPROPERTY()
    int32 Sequence = INDEX_NONE;

    UPROPERTY()
    int32 LoadedAmmo = 0;
};
