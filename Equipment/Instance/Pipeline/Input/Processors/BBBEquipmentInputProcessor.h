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
class FBBBEquipmentInputPipeline;

/** 解析外部输入并转发系统数据 */
class FBBBEquipmentInputProcessor final
{
private:
    friend class FBBBEquipmentInputPipeline;

    /** 处理本帧系统数据 */
    void Update(FBBBEquipmentInputRuntimeData &Input, FBBBEquipmentEquipRuntimeData &Equip,
        FBBBEquipmentFireRuntimeData &Fire, FBBBEquipmentReloadRuntimeData &Reload,
        FBBBCharacterInput &CharacterAPI, FName EquipmentId, bool bIsMirror) const;
};
