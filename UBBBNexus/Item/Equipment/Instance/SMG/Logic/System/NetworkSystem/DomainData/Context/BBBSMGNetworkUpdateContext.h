#pragma once
#include "CoreMinimal.h"
class ABBBCharacter;
class ABBBSMGEquipment;
class UBBBSMGDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBSMGRuntimeData;


/** Network 系统根本次调用的临时依赖 */
struct FBBBSMGNetworkUpdateContext final
{
    /** 本次 Equipment 依赖 */
    ABBBSMGEquipment &Equipment;
    /** 本次 Character 依赖 */
    ABBBCharacter &Character;
    /** 本次 RuntimeData 依赖 */
    FBBBSMGRuntimeData &RuntimeData;
    /** 本次 bCausal 依赖 */
    bool bCausal = true;
};
