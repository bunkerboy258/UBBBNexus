#pragma once
#include "CoreMinimal.h"
class ABBBMeleeEquipment;
class ABBBCharacter;
class USkeletalMeshComponent;
class UBBBMeleeDefinition;
class UWorld;
struct FBBBMeleeRuntimeData;
/** 本次 Parse 领域依赖 */
struct FBBBMeleeParseUpdateContext final
{
    /** 本次 RuntimeData 依赖 */
    FBBBMeleeRuntimeData &RuntimeData;
    /** 本次 bCausal 依赖 */
    bool bCausal;
};
