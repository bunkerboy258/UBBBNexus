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
class FBBBEquipmentInputPipeline;

/** 解析外部输入并转发系统数据 */
class FBBBEquipmentInputProcessor final
{
private:
    friend class FBBBEquipmentInputPipeline;

    /** 处理本帧系统数据 */
    void Update(FBBBEquipmentInputRuntimeData &Input, FBBBEquipmentEquipState &Equip,
        FBBBEquipmentFireState &Fire, FBBBEquipmentReloadState &Reload,
        FName EquipmentId, bool bIsMirror) const;
};
