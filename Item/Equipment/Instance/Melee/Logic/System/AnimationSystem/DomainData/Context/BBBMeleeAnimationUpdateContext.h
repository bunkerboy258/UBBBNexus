#pragma once
#include "CoreMinimal.h"
class ABBBMeleeEquipment;
class ABBBCharacter;
class USkeletalMeshComponent;
class UBBBMeleeDefinition;
class UWorld;
struct FBBBMeleeRuntimeData;
/** 本次 Animation 领域依赖 */
struct FBBBMeleeAnimationUpdateContext final
{
    /** 本次 Equipment 依赖 */
    ABBBMeleeEquipment &Equipment;
    /** 本次 Character 依赖 */
    ABBBCharacter &Character;
    /** 本次 Definition 依赖 */
    const UBBBMeleeDefinition &Definition;
    /** 本次 RuntimeData 依赖 */
    FBBBMeleeRuntimeData &RuntimeData;
    /** 本次 bCausal 依赖 */
    bool bCausal;
};
