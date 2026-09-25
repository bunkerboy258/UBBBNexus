#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ParseSystem/DomainData/Definitions/BBBRifleInputSlot.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/LocalControl/Reload/FBBBRifleDetachMagazineLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/LocalControl/Reload/FBBBRifleLoadMagazineLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/LocalControl/Reload/FBBBRifleInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/AuthorityFact/Action/FBBBRifleActionStateAuthorityFactPacket.h"

/** 步枪等待更新时消费的固定输入槽 */
struct FBBBRifleInputState final
{
    /** 等待消费的装备输入 */
    TBBBRifleInputSlot<FBBBEquipmentEquipLocalControlPacket> Equip;

    /** 等待消费的权威装备表现输入 */
    TBBBRifleInputSlot<FBBBEquipmentEquipAuthorityFactPacket> AuthorityEquip;

    /** 等待消费的开火输入 */
    TBBBRifleInputSlot<FBBBEquipmentPrimaryLocalControlPacket> Primary;

    /** 等待消费的换弹输入 */
    TBBBRifleInputSlot<FBBBEquipmentReloadLocalControlPacket> Reload;

    /** 等待消费的卸下弹匣输入 */
    TBBBRifleInputSlot<FBBBRifleDetachMagazineLocalControlPacket> DetachMagazine;

    /** 等待消费的装入弹匣输入 */
    TBBBRifleInputSlot<FBBBRifleLoadMagazineLocalControlPacket> LoadMagazine;

    /** 等待消费的结束换弹输入 */
    TBBBRifleInputSlot<FBBBRifleInterruptReloadLocalControlPacket> InterruptReload;

    /** 网络仅覆盖最新结果 */
    TBBBRifleInputSlot<FBBBRifleActionStateAuthorityFactPacket> AuthorityActionState;

private:
    friend struct FBBBRifleParseDomainState;
    FBBBRifleInputState() = default;
};
