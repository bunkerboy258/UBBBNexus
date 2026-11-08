#pragma once
#include "CoreMinimal.h"
class ABBBCharacter;
class ABBBRevolverEquipment;
class UBBBRevolverDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBRevolverRuntimeData;


/** Network 系统根本次调用的临时依赖 */
struct FBBBRevolverNetworkUpdateContext final
{
    /** 本次 Equipment 依赖 */
    ABBBRevolverEquipment &Equipment;
    /** 本次 Character 依赖 */
    ABBBCharacter &Character;
    /** 本次 RuntimeData 依赖 */
    FBBBRevolverRuntimeData &RuntimeData;
    /** 本次 bCausal 依赖 */
    bool bCausal = true;
};
