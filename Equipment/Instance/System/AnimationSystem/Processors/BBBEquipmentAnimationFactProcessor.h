#pragma once

#include "CoreMinimal.h"

class ABBBEquipment;
class UWorld;
class USkeletalMeshComponent;
struct FBBBEquipmentInputRuntimeData;
struct FBBBEquipmentEquipState;
struct FBBBEquipmentFireState;
struct FBBBEquipmentReloadState;
struct FBBBEquipmentAnimationState;
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
        FBBBEquipmentAnimationState &Data, const FBBBEquipmentFireState &Fire,
        const FBBBEquipmentReloadState &Reload,
        const FBBBEquipmentEquipFragment &EquipFragment, const FBBBEquipmentFireFragment &FireFragment) const;
};
