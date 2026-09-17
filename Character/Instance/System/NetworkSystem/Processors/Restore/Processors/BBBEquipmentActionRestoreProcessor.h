#pragma once

struct FBBBCharacterEquipmentCommands;
struct FBBBEquipmentActionNetworkPacket;

/** 将远端动作包还原为本帧装备命令 */
class FBBBEquipmentActionRestoreProcessor final
{
public:
    /**
     * 还原动作包
     * @param EquipmentCommands 装备命令
     * @param Packet 动作网络包
     */
    void Update(
        FBBBCharacterEquipmentCommands &EquipmentCommands,
        const FBBBEquipmentActionNetworkPacket &Packet) const;
};
