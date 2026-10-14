#pragma once
#include "CoreMinimal.h"
class ABBBCharacter;
class ABBBShotgunEquipment;
class UBBBShotgunDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBShotgunRuntimeData;


/** Network 系统根本次调用的临时依赖 */
struct FBBBShotgunNetworkUpdateContext final
{
    /** 本次 Equipment 依赖 */
    ABBBShotgunEquipment &Equipment;
    /** 本次 Character 依赖 */
    ABBBCharacter &Character;
    /** 本次 RuntimeData 依赖 */
    FBBBShotgunRuntimeData &RuntimeData;
    /** 本次 bCausal 依赖 */
    bool bCausal = true;
};
