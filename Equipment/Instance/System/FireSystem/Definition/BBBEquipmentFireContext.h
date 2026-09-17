#pragma once

#include "CoreMinimal.h"

class ABBBEquipment;
class FBBBCharacterInput;
class UWorld;
class USkeletalMeshComponent;

/** 开火片段执行时可访问的装备事实 */
struct FBBBEquipmentFireContext
{
    ABBBEquipment &Instance;
    UWorld &World;
    USkeletalMeshComponent &WeaponMesh;
    FBBBCharacterInput &CharacterAPI;
    int32 &LoadedAmmo;
    float &LastFireTimeSeconds;
    int32 Sequence;
    bool bIsMirror;
};
