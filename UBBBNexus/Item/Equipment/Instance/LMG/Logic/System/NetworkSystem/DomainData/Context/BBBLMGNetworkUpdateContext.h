#pragma once
#include "CoreMinimal.h"
class ABBBCharacter;
class ABBBLMGEquipment;
class UBBBLMGDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBLMGRuntimeData;


/** Network 系统根本次调用的临时依赖 */
struct FBBBLMGNetworkUpdateContext final
{
    /** 本次 Equipment 依赖 */
    ABBBLMGEquipment &Equipment;
    /** 本次 Character 依赖 */
    ABBBCharacter &Character;
    /** 本次 RuntimeData 依赖 */
    FBBBLMGRuntimeData &RuntimeData;
    /** 本次 bCausal 依赖 */
    bool bCausal = true;
};
