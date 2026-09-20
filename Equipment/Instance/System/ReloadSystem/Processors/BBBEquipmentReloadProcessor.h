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
class FBBBEquipmentReloadSystem;

/** Reload 系统的输入处理器 */
class FBBBEquipmentReloadProcessor final
{
private:
    friend class FBBBEquipmentReloadSystem;

    /** 处理本帧系统数据 */
    void Update(FBBBEquipmentReloadState &Data, FBBBEquipmentFireState &Fire,
        const FBBBEquipmentReloadFragment &Fragment, ABBBCharacter &CharacterAPI,
        FName EquipmentId, bool bIsMirror) const;
};
