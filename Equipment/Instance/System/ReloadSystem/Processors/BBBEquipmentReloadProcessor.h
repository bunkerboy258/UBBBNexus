#pragma once

#include "CoreMinimal.h"

class ABBBEquipment;
class ABBBCharacter;
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
class FBBBEquipmentReloadSystem;

/** Reload 系统的输入处理器 */
class FBBBEquipmentReloadProcessor final
{
private:
    friend class FBBBEquipmentReloadSystem;

    /** 处理本帧系统数据 */
    void Update(FBBBEquipmentReloadRuntimeData &Data, FBBBEquipmentFireRuntimeData &Fire,
        const FBBBEquipmentReloadFragment &Fragment, ABBBCharacter &CharacterAPI,
        FName EquipmentId, bool bIsMirror) const;
};
