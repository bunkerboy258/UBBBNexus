#pragma once
#include "CoreMinimal.h"
class ABBBCharacter;
class ABBBRevolverEquipment;
class UBBBRevolverDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBRevolverRuntimeData;


/** Parse 系统根本次调用的临时依赖 */
struct FBBBRevolverParseUpdateContext final
{
    /** 本次 RuntimeData 依赖 */
    FBBBRevolverRuntimeData &RuntimeData;
    /** 本次 bCausal 依赖 */
    bool bCausal = true;
};
