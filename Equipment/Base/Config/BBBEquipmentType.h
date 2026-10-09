#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentType.generated.h"

/** 当前装备配置的实际类别 */
UENUM(BlueprintType)
enum class EBBBEquipmentType : uint8
{
    /** 未配置或没有装备 */
    None UMETA(DisplayName = "无"),
    /** 使用枪口瞄准的步枪 */
    Rifle UMETA(DisplayName = "步枪"),
    /** 不使用枪口瞄准的近战装备 */
    Melee UMETA(DisplayName = "近战"),
    /** 手枪 */
    Pistol UMETA(DisplayName = "手枪"),
    /** 左轮 */
    Revolver UMETA(DisplayName = "左轮"),
    /** 霰弹枪 */
    Shotgun UMETA(DisplayName = "霰弹枪"),
    /** 冲锋枪 */
    SMG UMETA(DisplayName = "冲锋枪"),
    /** 狙击枪 */
    Sniper UMETA(DisplayName = "狙击枪"),
    /** 轻机枪 */
    LMG UMETA(DisplayName = "轻机枪"),
    /** 转管机枪 */
    Minigun UMETA(DisplayName = "转管机枪")
};
