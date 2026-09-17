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
class FBBBEquipmentEquipSystem;

/** Equip 系统的输入处理器 */
class FBBBEquipmentEquipProcessor final
{
private:
    friend class FBBBEquipmentEquipSystem;

    /** 处理本帧系统数据 */
    void Update(FBBBEquipmentEquipRuntimeData &Data, const FBBBEquipmentFireRuntimeData &Fire,
        const FBBBEquipmentEquipFragment &Fragment, FBBBCharacterExternalAPI &CharacterAPI,
        FName EquipmentId, bool bIsMirror) const;
};
