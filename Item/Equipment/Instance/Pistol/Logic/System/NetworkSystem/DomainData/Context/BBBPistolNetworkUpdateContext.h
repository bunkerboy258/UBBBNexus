#pragma once
#include "CoreMinimal.h"
class ABBBCharacter;
class ABBBPistolEquipment;
class UBBBPistolDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBPistolRuntimeData;


/** Network 系统根本次调用的临时依赖 */
struct FBBBPistolNetworkUpdateContext final
{
    /** 本次 Equipment 依赖 */
    ABBBPistolEquipment &Equipment;
    /** 本次 Character 依赖 */
    ABBBCharacter &Character;
    /** 本次 RuntimeData 依赖 */
    FBBBPistolRuntimeData &RuntimeData;
    /** 本次 bCausal 依赖 */
    bool bCausal = true;
};
