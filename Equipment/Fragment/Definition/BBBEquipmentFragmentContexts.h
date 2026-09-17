#pragma once

#include "CoreMinimal.h"

class ABBBEquipmentInstance;
class FBBBCharacterExternalAPI;
class UWorld;
class USkeletalMeshComponent;
struct FBBBEquipmentRuntimeData;
struct FBBBEquipmentEquipRuntimeData;
struct FBBBEquipmentAmmoConfig;

/** 装备片段执行时可访问的挂接事实 */
struct FBBBEquipmentEquipContext
{
    ABBBEquipmentInstance &Instance;
    USkeletalMeshComponent &CharacterMesh;
    USkeletalMeshComponent &WeaponMesh;
    FBBBEquipmentEquipRuntimeData &Runtime;
    FName AttachmentSocketName;
    FName MuzzleSocketName;
};

/** 开火片段执行时可访问的装备事实 */
struct FBBBEquipmentFireContext
{
    ABBBEquipmentInstance &Instance;
    UWorld &World;
    USkeletalMeshComponent &WeaponMesh;
    FBBBEquipmentRuntimeData &Runtime;
    FBBBCharacterExternalAPI &CharacterAPI;
    int32 Sequence;
    bool bIsMirror;
};

/** 换弹片段执行时可访问的装备事实 */
struct FBBBEquipmentReloadContext
{
    FBBBEquipmentRuntimeData &Runtime;
    FBBBCharacterExternalAPI &CharacterAPI;
    const FBBBEquipmentAmmoConfig &AmmoConfig;
    int32 Sequence;
};
