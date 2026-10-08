#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Config/Aim/BBBAimConfig.h"
#include "BBBWork/UBBBNexus/Character/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Character/Config/Equipment/BBBEquipmentConfig.h"
#include "BBBWork/UBBBNexus/Character/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBWork/UBBBNexus/Character/Config/Locomotion/BBBTraversalConfig.h"
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
    /** 正常生命上限与出生生命 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "生命", meta = (DisplayName = "最大生命", ClampMin = "1"))
    float MaximumHealth = 500.0f;

    /** 每次进入倒地状态的独立生命 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "生命", meta = (DisplayName = "倒地生命", ClampMin = "1"))
    float DownedHealth = 300.0f;

    /** 救援开始与维持的最大距离 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "生命|救援", meta = (DisplayName = "救援距离", ClampMin = "1", ToolTip = "双方固定脚底基准的最大距离"))
    float RescueDistance = 100.0f;

    /** 被救者接受后完成救援需要的时间 单位秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "生命|救援", meta = (DisplayName = "救援时长", ClampMin = "0.01", ToolTip = "由被救者控制端计时"))
    float RescueDuration = 3.0f;

    /** 救援成功后恢复的正常生命 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "生命|救援", meta = (DisplayName = "救援恢复生命", ClampMin = "1", ToolTip = "不超过角色最大生命"))
    float RescueHealth = 100.0f;

    /** 角色移动参数与碰撞配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "移动配置"))
    FBBBCharacterLocomotionConfig Locomotion;

    /** 独立的翻越探测和执行配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "翻越配置"))
    FBBBTraversalConfig Traversal;

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
