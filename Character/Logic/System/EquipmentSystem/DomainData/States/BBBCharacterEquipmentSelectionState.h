#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Template/BBBEquipment.h"
#include "BBBCharacterEquipmentSelectionState.generated.h"

/** 角色期望与当前主手装备状态 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentSelectionState final
{
    GENERATED_BODY()

    /** 当前激活装备的配置标识 */
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
