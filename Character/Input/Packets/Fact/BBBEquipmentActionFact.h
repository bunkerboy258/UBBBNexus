#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentActionFact.generated.h"

/**
 * 装备已确认动作的事实记录
 * 角色事件留档 网络传输与镜像快照共用的线上格式
 * PacketId 取自对应事实包类型的编译期常量
 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBEquipmentActionFact
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    uint8 PacketId = 0;

    UPROPERTY(BlueprintReadOnly)
    FName EquipmentId = NAME_None;

    UPROPERTY(BlueprintReadOnly)
    int32 Sequence = INDEX_NONE;

    UPROPERTY(BlueprintReadOnly)
    int32 LoadedAmmo = 0;
};
