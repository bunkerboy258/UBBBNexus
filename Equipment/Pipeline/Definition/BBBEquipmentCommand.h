#pragma once
#include "CoreMinimal.h"

/** 装备内部操作命令 */
enum class EBBBEquipmentCommandType : uint8
{
    /** 装备表现 */
    Equip,
    /** 开火 */
    Fire,
    /** 请求换弹 */
    Reload,
    /** 卸下弹夹 */
    DetachMagazine,
    /** 装填弹夹 */
    LoadMagazine,
    /** 取消换弹 */
    CancelReload
};

/** 装备内部待仲裁命令 */
struct FBBBEquipmentCommand
{
    EBBBEquipmentCommandType Type = EBBBEquipmentCommandType::Equip;
    int32 Sequence = INDEX_NONE;
};
