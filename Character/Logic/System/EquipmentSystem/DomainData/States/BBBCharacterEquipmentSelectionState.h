#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
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

    /** 等待处理的装备定义 空标识表示空手 */
    FName PendingEquipmentId = NAME_None;

    /** 当前是否存在装备创建或清空请求 */
    bool bHasEquipmentRequest = false;

    /** 等待处理的快捷栏索引 */
    TOptional<int32> PendingSlot;
};
