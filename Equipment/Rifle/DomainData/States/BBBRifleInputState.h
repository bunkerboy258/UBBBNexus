#pragma once

#include "BBBWork/UBBBNexus/Equipment/Template/Input/BBBEquipmentInputSlot.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleDetachMagazineInput.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleEquipInput.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleFireInput.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleInterruptReloadInput.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleLoadMagazineInput.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleReloadInput.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleRestoreFactInput.h"
#include "Containers/StaticArray.h"

/** 步枪等待固定时机解析的输入状态 */
struct FBBBRifleInputState final
{
    /** 单次更新允许积累的镜像事实数量 */
    static constexpr int32 MaxRestoreFactCount = 8;

    /** 按提交顺序保存的镜像事实 */
    TStaticArray<FBBBRifleRestoreFactInput, MaxRestoreFactCount> RestoreFacts;

    /** 当前有效镜像事实数量 */
    int32 RestoreFactCount = 0;

    /** 等待解析的装备输入 */
    TBBBEquipmentInputSlot<FBBBRifleEquipInput> Equip;

    /** 等待解析的卸下弹匣输入 */
    TBBBEquipmentInputSlot<FBBBRifleDetachMagazineInput> DetachMagazine;

    /** 等待解析的装入弹匣输入 */
    TBBBEquipmentInputSlot<FBBBRifleLoadMagazineInput> LoadMagazine;

    /** 等待解析的结束换弹输入 */
    TBBBEquipmentInputSlot<FBBBRifleInterruptReloadInput> InterruptReload;

    /** 等待解析的开始换弹输入 */
    TBBBEquipmentInputSlot<FBBBRifleReloadInput> Reload;

    /** 等待解析的开火输入 */
    TBBBEquipmentInputSlot<FBBBRifleFireInput> Fire;

private:
    friend struct FBBBRifleDomainState;

    FBBBRifleInputState() = default;
    FBBBRifleInputState(const FBBBRifleInputState &) = default;
    FBBBRifleInputState &operator=(const FBBBRifleInputState &) = default;
};
