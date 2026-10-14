#pragma once
#include "CoreMinimal.h"
class ABBBCharacter;
class ABBBMinigunEquipment;
class UBBBMinigunDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBMinigunRuntimeData;


/** Network 系统根本次调用的临时依赖 */
struct FBBBMinigunNetworkUpdateContext final
{
    /** 本次 Equipment 依赖 */
    ABBBMinigunEquipment &Equipment;
    /** 本次 Character 依赖 */
    ABBBCharacter &Character;
    /** 本次 RuntimeData 依赖 */
    FBBBMinigunRuntimeData &RuntimeData;
    /** 本次 bCausal 依赖 */
    bool bCausal = true;
};
