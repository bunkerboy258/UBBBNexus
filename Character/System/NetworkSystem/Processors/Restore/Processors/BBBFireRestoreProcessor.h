
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterEquipmentCommands;
struct FBBBFireNetworkPacket;

class ABBB_EVAC_API FBBBFireRestoreProcessor final
{
public:

    /**
     * 将远端开火事件还原为本地开火表现指令
     * @param EquipmentCommands	角色物品指令
     * @param Packet	开火网络Packet
     */
    void Update(
        FBBBCharacterEquipmentCommands &EquipmentCommands,
        const FBBBFireNetworkPacket &Packet) const;
};
