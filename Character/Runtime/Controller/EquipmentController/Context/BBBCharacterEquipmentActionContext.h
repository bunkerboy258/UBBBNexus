#pragma once

#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipmentActionFact.h"

class ABBBEquipment;

/** 本次装备动作解析与提交使用的栈上暂存数据 */
struct FBBBCharacterEquipmentActionContext final
{
    /** 当前主手装备 */
    ABBBEquipment *Equipment = nullptr;

    /** 本次等待镜像还原的装备事实 */
    TArray<FBBBEquipmentActionFact> Facts;

    /** 本次是否提交主操作 */
    bool bFire = false;

    /** 本次是否提交换弹 */
    bool bReload = false;

    /** 本次是否提交副操作状态 */
    bool bSecondarySubmitted = false;

    /** 本次副操作状态 */
    bool bSecondaryActive = false;
};
