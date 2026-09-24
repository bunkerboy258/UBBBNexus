#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Input/BBBEquipmentInputSlot.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/Shared/Action/BBBRifleEquipPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/Local/Action/BBBRifleFirePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/Local/Action/BBBRifleReloadPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/Local/Action/BBBRifleDetachMagazinePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/Local/Action/BBBRifleLoadMagazinePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/Local/Action/BBBRifleInterruptReloadPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/Mirror/State/BBBRifleNetworkStatePacket.h"

/** 步枪等待更新时消费的固定输入槽 */
struct FBBBRifleInputState final
{
    /** 等待消费的装备输入 */
    TBBBEquipmentInputSlot<FBBBRifleEquipPacket> Equip;

    /** 等待消费的开火输入 */
    TBBBEquipmentInputSlot<FBBBRifleFirePacket> Fire;

    /** 等待消费的换弹输入 */
    TBBBEquipmentInputSlot<FBBBRifleReloadPacket> Reload;

    /** 等待消费的卸下弹匣输入 */
    TBBBEquipmentInputSlot<FBBBRifleDetachMagazinePacket> DetachMagazine;

    /** 等待消费的装入弹匣输入 */
    TBBBEquipmentInputSlot<FBBBRifleLoadMagazinePacket> LoadMagazine;

    /** 等待消费的结束换弹输入 */
    TBBBEquipmentInputSlot<FBBBRifleInterruptReloadPacket> InterruptReload;

    /** 网络仅覆盖最新结果 */
    TBBBEquipmentInputSlot<FBBBRifleNetworkStatePacket> NetworkState;

private:
    friend struct FBBBRifleParseDomainState;
    FBBBRifleInputState() = default;
};
