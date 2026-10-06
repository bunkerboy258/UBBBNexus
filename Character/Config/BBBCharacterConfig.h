#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Config/Aim/BBBAimConfig.h"
#include "BBBWork/UBBBNexus/Character/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Character/Config/Equipment/BBBEquipmentConfig.h"
#include "BBBWork/UBBBNexus/Character/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBWork/UBBBNexus/Character/Config/Network/BBBNetworkConfig.h"
#include "Engine/DataAsset.h"
#include "BBBWork/UBBBNexus/Character/Config/Item/BBBItemConfig.h"
#include "BBBCharacterConfig.generated.h"

/** 角色全部可编辑的静态配置资产 */
UCLASS(BlueprintType)
//聚合角色全部可编辑运行配置
class ABBB_EVAC_API UBBBCharacterConfig final : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    /** 角色移动参数与碰撞配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "移动配置"))
    FBBBCharacterLocomotionConfig Locomotion;

    /** 角色瞄准动画配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "瞄准动画"))
    FBBBAimAnimationConfig AimAnimation;

    /** 角色动画事实识别配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "动画"))
    FBBBCharacterAnimationConfig Animation;

    /** 角色背包与前序快捷槽位配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "物品"))
    FBBBCharacterItemConfig Item;

    /** 角色装备目录与容器配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "装备"))
    FBBBCharacterEquipmentConfig Equipment;

    /** 角色网络状态同步配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "网络配置"))
    FBBBCharacterNetworkConfig Network;
};
