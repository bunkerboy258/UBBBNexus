#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pipeline/Input/Processors/BBBEquipmentInputProcessor.h"

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
class FBBBEquipmentInitializer;
class FBBBEquipmentUpdatePipeline;

/** 统一装备外部输入 */
class FBBBEquipmentInputPipeline final
{
private:
    friend class FBBBEquipmentInitializer;
    friend class FBBBEquipmentUpdatePipeline;

    /** 注入本系统所需的数据与行为配置 */
    void Initialize(FBBBEquipmentInputRuntimeData &InInput, FBBBEquipmentEquipState &InEquip,
        FBBBEquipmentFireState &InFire, FBBBEquipmentReloadState &InReload,
        FName InEquipmentId, bool bInIsMirror);

    /** 更新本系统 */
    void Update() const;

    FBBBEquipmentInputRuntimeData *Input = nullptr;
    FBBBEquipmentEquipState *Equip = nullptr;
    FBBBEquipmentFireState *Fire = nullptr;
    FBBBEquipmentReloadState *Reload = nullptr;
    FName EquipmentId;
    bool bIsMirror = false;

    FBBBEquipmentInputProcessor Processor;
};
