#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterPhysicalPresentationState.generated.h"

/** 局部受击与全身物理表现的独立应用记录 */
USTRUCT()
struct FBBBCharacterPhysicalPresentationState final
{
    GENERATED_BODY()
    /** 最新已经表现的命中 */
    uint64 HitSerial = 0;
    /** 全身布娃娃是否已经接管网格 */
    bool bRagdoll = false;
};
