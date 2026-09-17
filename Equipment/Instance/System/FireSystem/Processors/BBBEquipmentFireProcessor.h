#pragma once

#include "CoreMinimal.h"

class ABBBEquipment;
class FBBBCharacterInput;
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
class FBBBEquipmentFireSystem;

/** Fire 系统的输入处理器 */
class FBBBEquipmentFireProcessor final
{
private:
    friend class FBBBEquipmentFireSystem;

    /** 处理本帧系统数据 */
    void Update(ABBBEquipment &Instance, USkeletalMeshComponent &WeaponMesh,
        FBBBEquipmentFireRuntimeData &Data, const FBBBEquipmentReloadRuntimeData &Reload,
        const FBBBEquipmentFireFragment &Fragment, FBBBCharacterInput &CharacterAPI,
        FName EquipmentId, bool bIsMirror) const;
};
