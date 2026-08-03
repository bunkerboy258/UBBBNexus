#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Context/BBBCharacterItemActionResults.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Context/BBBCharacterItemCommands.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Context/BBBCharacterItemStates.h"
#include "BBBCharacterItemRuntimeData.generated.h"

class FBBBCharacterDefaultItemInitializer;
class FBBBCharacterInitializer;
class FBBBCharacterItemSystem;

/** 角色物品领域唯一数据根 */
USTRUCT(BlueprintType)
struct FBBBCharacterItemRuntimeData
{
    GENERATED_BODY()

    /** 清理本帧物品命令与动作结果 */
    void CleanFrame()
    {
        Commands.CleanFrame();
        ActionResults.CleanFrame();
    }

private:

    friend class FBBBCharacterDefaultItemInitializer;
    friend class FBBBCharacterInitializer;
    friend class FBBBCharacterItemSystem;

    /** 角色唯一背包 */
    UPROPERTY()
    FBBBCharacterBackpackState Backpack;

    /** 角色当前装备状态 */
    UPROPERTY()
    FBBBCharacterEquipmentState Equipment;

    /** 本帧物品执行命令 */
    UPROPERTY()
    FBBBCharacterItemCommands Commands;

    /** 本帧物品动作结果 */
    UPROPERTY()
    FBBBCharacterItemActionResults ActionResults;
};
