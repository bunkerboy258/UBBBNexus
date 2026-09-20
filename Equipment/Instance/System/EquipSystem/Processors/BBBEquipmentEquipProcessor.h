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
class FBBBEquipmentEquipSystem;

/** Equip 系统的输入处理器 */
class FBBBEquipmentEquipProcessor final
{
private:
    friend class FBBBEquipmentEquipSystem;

    /** 处理本帧系统数据 */
    void Update(FBBBEquipmentEquipState &Data, const FBBBEquipmentFireState &Fire,
        const FBBBEquipmentEquipFragment &Fragment, ABBBCharacter &CharacterAPI,
        FName EquipmentId, bool bIsMirror) const;
};
