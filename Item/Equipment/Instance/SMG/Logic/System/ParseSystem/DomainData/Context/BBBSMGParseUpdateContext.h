#pragma once
#include "CoreMinimal.h"
class ABBBCharacter;
class ABBBSMGEquipment;
class UBBBSMGDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBSMGRuntimeData;


/** Parse 系统根本次调用的临时依赖 */
struct FBBBSMGParseUpdateContext final
{
    /** 本次 RuntimeData 依赖 */
    FBBBSMGRuntimeData &RuntimeData;
    /** 本次 bCausal 依赖 */
    bool bCausal = true;
};
