#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Results/BBBCharacterEquipmentResults.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBCharacterEquipmentRuntimeData.generated.h"

class FBBBCharacterDefaultEquipmentInitializer;
class FBBBCharacterEquipmentSystem;
class FBBBCharacterInitializer;

/** 角色装备领域唯一数据根 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentRuntimeData
{
    GENERATED_BODY()

    /** 清理本帧装备命令与结果 */
    void CleanFrame()
    {
        Commands.CleanFrame();
        Results.CleanFrame();
    }

private:
    friend class FBBBCharacterDefaultEquipmentInitializer;
    friend class FBBBCharacterEquipmentSystem;
    friend class FBBBCharacterInitializer;

    /** 角色装备容器 */
    UPROPERTY()
    FBBBCharacterEquipmentInventoryState Inventory;

    /** 角色当前装备状态 */
    UPROPERTY()
    FBBBCharacterEquipmentState Equipment;

    /** 本帧装备执行命令 */
    UPROPERTY()
    FBBBCharacterEquipmentCommands Commands;

    /** 本帧装备执行结果 */
    UPROPERTY()
    FBBBCharacterEquipmentResults Results;
};
