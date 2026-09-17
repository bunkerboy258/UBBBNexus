#pragma once

#include "CoreMinimal.h"

class ABBBEquipmentInstance;
class FBBBCharacterExternalAPI;
class UWorld;
class USkeletalMeshComponent;

/** 开火片段执行时可访问的装备事实 */
struct FBBBEquipmentFireContext
{
    ABBBEquipmentInstance &Instance;
    UWorld &World;
    USkeletalMeshComponent &WeaponMesh;
    FBBBCharacterExternalAPI &CharacterAPI;
    int32 &LoadedAmmo;
    float &LastFireTimeSeconds;
    int32 Sequence;
    bool bIsMirror;
};
