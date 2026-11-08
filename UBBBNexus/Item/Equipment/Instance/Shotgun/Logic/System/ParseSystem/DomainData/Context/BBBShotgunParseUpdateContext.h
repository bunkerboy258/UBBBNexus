#pragma once
#include "CoreMinimal.h"
class ABBBCharacter;
class ABBBShotgunEquipment;
class UBBBShotgunDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBShotgunRuntimeData;


/** Parse 系统根本次调用的临时依赖 */
struct FBBBShotgunParseUpdateContext final
{
    /** 本次 RuntimeData 依赖 */
    FBBBShotgunRuntimeData &RuntimeData;
    /** 本次 bCausal 依赖 */
    bool bCausal = true;
};
