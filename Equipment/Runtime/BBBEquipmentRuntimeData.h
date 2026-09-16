#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Runtime/Definition/Equip/BBBEquipmentEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Runtime/Definition/Fire/BBBEquipmentFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Runtime/Definition/Reload/BBBEquipmentReloadRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Runtime/Definition/Ammo/BBBEquipmentAmmoRuntimeData.h"
#include "BBBEquipmentRuntimeData.generated.h"

/** 装备实例独占的运行数据根 */
USTRUCT()
struct FBBBEquipmentRuntimeData
{
    GENERATED_BODY()

    FBBBEquipmentEquipRuntimeData Equip;
    FBBBEquipmentFireRuntimeData Fire;
    FBBBEquipmentReloadRuntimeData Reload;
    FBBBEquipmentAmmoRuntimeData Ammo;
};
