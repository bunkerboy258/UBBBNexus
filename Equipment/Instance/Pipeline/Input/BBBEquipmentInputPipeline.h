#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pipeline/Input/Processors/BBBEquipmentInputProcessor.h"

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
class FBBBEquipmentInitializer;
class FBBBEquipmentUpdatePipeline;

/** 统一装备外部输入 */
class FBBBEquipmentInputPipeline final
{
private:
    friend class FBBBEquipmentInitializer;
    friend class FBBBEquipmentUpdatePipeline;

    /** 注入本系统所需的数据与行为配置 */
    void Initialize(FBBBEquipmentInputRuntimeData &InInput, FBBBEquipmentEquipRuntimeData &InEquip,
        FBBBEquipmentFireRuntimeData &InFire, FBBBEquipmentReloadRuntimeData &InReload,
        FName InEquipmentId, bool bInIsMirror);

    /** 更新本系统 */
    void Update() const;

    FBBBEquipmentInputRuntimeData *Input = nullptr;
    FBBBEquipmentEquipRuntimeData *Equip = nullptr;
    FBBBEquipmentFireRuntimeData *Fire = nullptr;
    FBBBEquipmentReloadRuntimeData *Reload = nullptr;
    FName EquipmentId;
    bool bIsMirror = false;

    FBBBEquipmentInputProcessor Processor;
};
