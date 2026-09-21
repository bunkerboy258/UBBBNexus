#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBCharacterEquipmentStates.generated.h"

/** 角色拥有的装备及快捷访问绑定 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentInventoryState
{
    GENERATED_BODY()

    /** 角色拥有的全部装备实例 */
    UPROPERTY(BlueprintReadOnly)
    TArray<TObjectPtr<ABBBEquipment>> Slots;

    /** 直接引用装备实例的快捷操作槽位 */
    UPROPERTY(BlueprintReadOnly)
    TArray<TObjectPtr<ABBBEquipment>> QuickAccessBindings;
};

/** 角色期望与当前主手装备状态 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentState
{
    GENERATED_BODY()

    FName ActiveEquipmentId = NAME_None;
    /** 角色期望装备的唯一实例 */
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<ABBBEquipment> DesiredMainHandInstance = nullptr;

    /** 角色当前装备的唯一实例 */
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<ABBBEquipment> ActiveMainHandInstance = nullptr;

    /** 下一个本地动作顺序号 */
    int32 NextActionSequence = 1;
};
