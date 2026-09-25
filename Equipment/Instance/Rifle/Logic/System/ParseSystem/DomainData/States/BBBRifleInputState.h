#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ParseSystem/DomainData/Definitions/BBBRifleInputSlot.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/Shared/Action/BBBEquipmentEquipPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/Local/Action/BBBEquipmentPrimaryPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/Local/Action/BBBEquipmentReloadPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/Local/Action/BBBRifleDetachMagazinePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/Local/Action/BBBRifleLoadMagazinePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/Local/Action/BBBRifleInterruptReloadPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/Mirror/State/BBBRifleNetworkStatePacket.h"

/** 步枪等待更新时消费的固定输入槽 */
struct FBBBRifleInputState final
{
    /** 等待消费的装备输入 */
    TBBBRifleInputSlot<FBBBEquipmentEquipPacket> Equip;

    /** 等待消费的开火输入 */
    TBBBRifleInputSlot<FBBBEquipmentPrimaryPacket> Primary;

    /** 等待消费的换弹输入 */
    TBBBRifleInputSlot<FBBBEquipmentReloadPacket> Reload;

    /** 等待消费的卸下弹匣输入 */
    TBBBRifleInputSlot<FBBBRifleDetachMagazinePacket> DetachMagazine;

    /** 等待消费的装入弹匣输入 */
    TBBBRifleInputSlot<FBBBRifleLoadMagazinePacket> LoadMagazine;

    /** 等待消费的结束换弹输入 */
    TBBBRifleInputSlot<FBBBRifleInterruptReloadPacket> InterruptReload;

    /** 网络仅覆盖最新结果 */
    TBBBRifleInputSlot<FBBBRifleNetworkStatePacket> NetworkState;

private:
    friend struct FBBBRifleParseDomainState;
    FBBBRifleInputState() = default;
};
