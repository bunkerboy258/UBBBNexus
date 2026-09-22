#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Template/BBBEquipment.h"
#include "BBBCharacterEquipmentInventoryState.generated.h"

/** 角色拥有的装备与快捷访问绑定状态 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentInventoryState final
{
    GENERATED_BODY()

    /** 角色拥有的全部装备实例 */
    UPROPERTY(BlueprintReadOnly)
    TArray<TObjectPtr<ABBBEquipment>> Slots;

    /** 直接引用装备实例的快捷操作槽位 */
    UPROPERTY(BlueprintReadOnly)
    TArray<TObjectPtr<ABBBEquipment>> QuickAccessBindings;
};
