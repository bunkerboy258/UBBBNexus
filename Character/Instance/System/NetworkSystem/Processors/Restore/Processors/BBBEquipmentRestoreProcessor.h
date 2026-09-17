
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterEquipmentCommands;
struct FBBBEquipmentNetworkPacket;
class UBBBEquipmentCatalog;

class ABBB_EVAC_API FBBBEquipmentRestoreProcessor final
{
public:

    /**
     * 把网络装备句柄转换为角色装备命令
     * @param EquipmentCommands 角色装备命令
     * @param Packet            装备网络Packet
     * @param EquipmentCatalog  装备目录
     */
    void Update(
        FBBBCharacterEquipmentCommands &EquipmentCommands,
        const FBBBEquipmentNetworkPacket &Packet,
        UBBBEquipmentCatalog &EquipmentCatalog) const;
};
