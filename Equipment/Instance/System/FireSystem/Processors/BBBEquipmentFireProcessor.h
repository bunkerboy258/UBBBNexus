#pragma once

#include "CoreMinimal.h"

class ABBBEquipment;
class ABBBCharacter;
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
class FBBBEquipmentFireSystem;

/** Fire 系统的输入处理器 */
class FBBBEquipmentFireProcessor final
{
private:
    friend class FBBBEquipmentFireSystem;

    /** 处理本帧系统数据 */
    void Update(ABBBEquipment &Instance, USkeletalMeshComponent &WeaponMesh,
        FBBBEquipmentFireState &Data, const FBBBEquipmentReloadState &Reload,
        const FBBBEquipmentFireFragment &Fragment, ABBBCharacter &CharacterAPI,
        FName EquipmentId, bool bIsMirror) const;
};
