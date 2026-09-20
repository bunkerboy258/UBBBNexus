#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipmentActionFact.h"

/** 网络确认后的装备事实输入 */
struct FBBBEquipmentFactInput final
{
    /** 已成立且允许镜像还原的装备事实 */
    FBBBEquipmentActionFact Fact;
};

/** 装备进入当前手持槽位的输入 */
struct FBBBEquipmentEquipInput final
{
    /** 角色领域分配的动作序号 */
    int32 Sequence = INDEX_NONE;
};

/** 装备副操作输入 */
struct FBBBEquipmentSecondaryInput final
{
    /** 副操作当前是否处于按下状态 */
    bool bActive = false;

    /** 角色领域分配的动作序号 */
    int32 Sequence = INDEX_NONE;
};

/** 弹匣脱离输入 */
struct FBBBEquipmentDetachMagazineInput final
{
    /** 目标换弹流程序号，INDEX_NONE 表示使用当前流程 */
    int32 Sequence = INDEX_NONE;
};

/** 弹匣装入输入 */
struct FBBBEquipmentLoadMagazineInput final
{
    /** 目标换弹流程序号，INDEX_NONE 表示使用当前流程 */
    int32 Sequence = INDEX_NONE;
};

/** 换弹打断输入 */
struct FBBBEquipmentInterruptReloadInput final
{
    /** 目标换弹流程序号，INDEX_NONE 表示使用当前流程 */
    int32 Sequence = INDEX_NONE;
};

/** 开始换弹输入 */
struct FBBBEquipmentReloadInput final
{
    /** 角色领域分配的动作序号 */
    int32 Sequence = INDEX_NONE;
};

/** 装备主操作输入 */
struct FBBBEquipmentPrimaryInput final
{
    /** 角色领域分配的动作序号 */
    int32 Sequence = INDEX_NONE;
};
