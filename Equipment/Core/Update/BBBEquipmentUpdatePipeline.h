#pragma once

#include "CoreMinimal.h"

class ABBBEquipmentInstance;
class FBBBEquipmentInitializer;
class FBBBEquipmentInputPipeline;
class FBBBEquipmentEquipSystem;
class FBBBEquipmentReloadSystem;
class FBBBEquipmentFireSystem;
class FBBBEquipmentAnimationSystem;
struct FBBBEquipmentRuntimeData;

/** 装备实例自主驱动的唯一更新管线 */
class ABBB_EVAC_API FBBBEquipmentUpdatePipeline final
{
private:
    friend class ABBBEquipmentInstance;
    friend class FBBBEquipmentInitializer;

    /** 注入固定系统和黑板 */
    void Initialize(FBBBEquipmentRuntimeData &InRuntimeData, FBBBEquipmentInputPipeline &InInput,
        FBBBEquipmentEquipSystem &InEquip, FBBBEquipmentReloadSystem &InReload,
        FBBBEquipmentFireSystem &InFire, FBBBEquipmentAnimationSystem &InAnimation);

    /** 按固定顺序调度各系统 */
    void Update() const;

    FBBBEquipmentRuntimeData *RuntimeData = nullptr;
    FBBBEquipmentInputPipeline *Input = nullptr;
    FBBBEquipmentEquipSystem *Equip = nullptr;
    FBBBEquipmentReloadSystem *Reload = nullptr;
    FBBBEquipmentFireSystem *Fire = nullptr;
    FBBBEquipmentAnimationSystem *Animation = nullptr;
};
