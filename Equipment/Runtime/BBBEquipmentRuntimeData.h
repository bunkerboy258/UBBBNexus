#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Pipeline/Input/Definition/BBBEquipmentInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/System/EquipSystem/Definition/BBBEquipmentEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/System/FireSystem/Definition/BBBEquipmentFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/System/ReloadSystem/Definition/BBBEquipmentReloadRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/System/AnimationSystem/Definition/BBBEquipmentAnimationRuntimeData.h"
#include "BBBEquipmentRuntimeData.generated.h"

class FBBBEquipmentInitializer;
class FBBBEquipmentUpdatePipeline;

/** 装备实例独占的运行数据根 */
USTRUCT()
struct FBBBEquipmentRuntimeData
{
    GENERATED_BODY()

private:
    friend class FBBBEquipmentInitializer;
    friend class FBBBEquipmentUpdatePipeline;

    /** 清理已消费的本帧系统输入，保留新到达的外部输入 */
    void CleanFrame();

    UPROPERTY(Transient)
    FBBBEquipmentInputRuntimeData Input;

    UPROPERTY(Transient)
    FBBBEquipmentEquipRuntimeData Equip;

    UPROPERTY(Transient)
    FBBBEquipmentFireRuntimeData Fire;

    UPROPERTY(Transient)
    FBBBEquipmentReloadRuntimeData Reload;

    UPROPERTY(Transient)
    FBBBEquipmentAnimationRuntimeData Animation;
};
