
#pragma once
#include "CoreMinimal.h"
#include "BBBItemTypes.generated.h"

UENUM(BlueprintType)
enum class EBBBEquipmentSlot : uint8
{
    MainHand UMETA(DisplayName = "Main Hand")
};

UENUM(BlueprintType)
enum class EBBBWeaponAnimType : uint8
{
    None UMETA(DisplayName = "None"),
    Rifle UMETA(DisplayName = "Rifle")
};

/** 装备实体的数据完整度 */
UENUM()
enum class EBBBEquipmentInstanceMode : uint8
{
    /** 持有完整运行数据的本地装备 */
    Runtime,

    /** 仅响应远端表现指令的装备镜像 */
    Mirror
};

/** 角色当前期望生成的装备类型 */
UENUM()
enum class EBBBEquipmentTargetMode : uint8
{
    /** 当前不期望持有装备 */
    None,

    /** 由本地物品实例生成装备 */
    RuntimeItem,

    /** 由网络句柄生成装备镜像 */
    Mirror
};
