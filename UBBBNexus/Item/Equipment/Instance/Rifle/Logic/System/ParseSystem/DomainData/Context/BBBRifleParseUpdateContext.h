#pragma once
#include "CoreMinimal.h"
class ABBBCharacter;
class ABBBRifleEquipment;
class UBBBRifleDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBRifleRuntimeData;


/** Parse 系统根本次调用的临时依赖 */
struct FBBBRifleParseUpdateContext final
{
    /** 本次 RuntimeData 依赖 */
    FBBBRifleRuntimeData &RuntimeData;
    /** 本次 bCausal 依赖 */
    bool bCausal = true;
};
