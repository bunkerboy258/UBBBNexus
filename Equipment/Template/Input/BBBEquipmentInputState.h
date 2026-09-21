#pragma once

#include "BBBWork/UBBBNexus/Equipment/Template/Input/BBBEquipmentInputPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Input/BBBEquipmentInputSlot.h"

/** 所有装备实例共享的固定输入状态 */
struct FBBBEquipmentInputState final
{
    /** 网络确认事实 */
    TBBBEquipmentInputSlot<FBBBEquipmentFactInput> Fact;

    /** 装备动作 */
    TBBBEquipmentInputSlot<FBBBEquipmentEquipInput> Equip;

    /** 副操作 */
    TBBBEquipmentInputSlot<FBBBEquipmentSecondaryInput> Secondary;

    /** 弹匣脱离 */
    TBBBEquipmentInputSlot<FBBBEquipmentDetachMagazineInput> DetachMagazine;

    /** 弹匣装入 */
    TBBBEquipmentInputSlot<FBBBEquipmentLoadMagazineInput> LoadMagazine;

    /** 换弹打断 */
    TBBBEquipmentInputSlot<FBBBEquipmentInterruptReloadInput> InterruptReload;

    /** 开始换弹 */
    TBBBEquipmentInputSlot<FBBBEquipmentReloadInput> Reload;

    /** 主操作 */
    TBBBEquipmentInputSlot<FBBBEquipmentPrimaryInput> Primary;
};
