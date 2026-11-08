#pragma once

#include "CoreMinimal.h"
#include "BBBMonsterSpecialKind.generated.h"

/** 特感使用的具体战斗方式 */
UENUM(BlueprintType)
enum class EBBBMonsterSpecialKind : uint8
{
    None,
    Charger,
    Spitter,
    Stalker
};
