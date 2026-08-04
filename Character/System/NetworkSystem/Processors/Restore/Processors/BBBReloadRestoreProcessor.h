
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterEquipmentCommands;
struct FBBBReloadNetworkPacket;

class ABBB_EVAC_API FBBBReloadRestoreProcessor final
{
public:

    /**
     * 将远端换弹事件还原为本地换弹表现指令
     * @param EquipmentCommands	角色物品指令
     * @param Packet	换弹网络Packet
     */
    void Update(
        FBBBCharacterEquipmentCommands &EquipmentCommands,
        const FBBBReloadNetworkPacket &Packet) const;
};
