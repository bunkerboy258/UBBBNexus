#pragma once

#include "CoreMinimal.h"

class ABBBEquipment;
class FBBBCharacterExternalAPI;
class UWorld;
class USkeletalMeshComponent;
struct FBBBEquipmentInputRuntimeData;
struct FBBBEquipmentEquipRuntimeData;
struct FBBBEquipmentFireRuntimeData;
struct FBBBEquipmentReloadRuntimeData;
struct FBBBEquipmentAnimationRuntimeData;
struct FBBBEquipmentEquipFragment;
struct FBBBEquipmentFireFragment;
struct FBBBEquipmentReloadFragment;
class FBBBEquipmentAnimationSystem;

/** 计算装备动画事实 */
class FBBBEquipmentAnimationFactProcessor final
{
private:
    friend class FBBBEquipmentAnimationSystem;

    /** 处理本帧系统数据 */
    void Update(USkeletalMeshComponent &CharacterMesh, USkeletalMeshComponent &WeaponMesh,
        FBBBEquipmentAnimationRuntimeData &Data, const FBBBEquipmentFireRuntimeData &Fire,
        const FBBBEquipmentEquipFragment &EquipFragment, const FBBBEquipmentFireFragment &FireFragment) const;
};
