#pragma once
#include "CoreMinimal.h"
class ABBBCharacter;
class ABBBLMGEquipment;
class UBBBLMGDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBLMGRuntimeData;


/** Parse 系统根本次调用的临时依赖 */
struct FBBBLMGParseUpdateContext final
{
    /** 本次 RuntimeData 依赖 */
    FBBBLMGRuntimeData &RuntimeData;
    /** 本次 bCausal 依赖 */
    bool bCausal = true;
};
