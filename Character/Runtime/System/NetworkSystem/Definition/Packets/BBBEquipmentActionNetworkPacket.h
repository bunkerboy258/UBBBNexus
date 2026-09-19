#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentActionNetworkPacket.generated.h"

/** 单次装备动作网络包 PacketId 取自对应事实包类型的编译期常量 */
USTRUCT()
struct FBBBEquipmentActionNetworkPacket
{
    GENERATED_BODY()

    UPROPERTY()
    uint8 PacketId = 0;

    UPROPERTY()
    FName EquipmentId = NAME_None;

    UPROPERTY()
    int32 Sequence = 0;

    UPROPERTY()
    int32 LoadedAmmo = 0;
};
