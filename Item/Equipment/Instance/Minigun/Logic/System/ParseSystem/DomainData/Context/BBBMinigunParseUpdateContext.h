#pragma once
#include "CoreMinimal.h"
class ABBBCharacter;
class ABBBMinigunEquipment;
class UBBBMinigunDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBMinigunRuntimeData;


/** Parse 系统根本次调用的临时依赖 */
struct FBBBMinigunParseUpdateContext final
{
    /** 本次 RuntimeData 依赖 */
    FBBBMinigunRuntimeData &RuntimeData;
    /** 本次 bCausal 依赖 */
    bool bCausal = true;
};
