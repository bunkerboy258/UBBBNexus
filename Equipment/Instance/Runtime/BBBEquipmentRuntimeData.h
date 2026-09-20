#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pipeline/Input/Definition/BBBEquipmentInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/EquipSystem/State/BBBEquipmentEquipState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/FireSystem/State/BBBEquipmentFireState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/ReloadSystem/State/BBBEquipmentReloadState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/AnimationSystem/State/BBBEquipmentAnimationState.h"
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
    FBBBEquipmentEquipState Equip;

    UPROPERTY(Transient)
    FBBBEquipmentFireState Fire;

    UPROPERTY(Transient)
    FBBBEquipmentReloadState Reload;

    UPROPERTY(Transient)
    FBBBEquipmentAnimationState Animation;
};
