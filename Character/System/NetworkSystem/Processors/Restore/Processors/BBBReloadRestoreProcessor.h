
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterItemCommands;
struct FBBBReloadNetworkPacket;

class ABBB_EVAC_API FBBBReloadRestoreProcessor final
{
public:

    /**
     * 将远端换弹事件还原为本地换弹表现指令
     * @param ItemCommands	角色物品指令
     * @param Packet	换弹网络Packet
     */
    void Update(
        FBBBCharacterItemCommands &ItemCommands,
        const FBBBReloadNetworkPacket &Packet) const;
};
