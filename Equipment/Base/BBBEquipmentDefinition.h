#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/BBBEquipDomin.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/BBBFireDomin.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Magazine/BBBMagazineDomin.h"
#include "Engine/DataAsset.h"
#include "StructUtils/InstancedStruct.h"
#include "BBBEquipmentDefinition.generated.h"

class UTexture2D;

/** 装备静态配置与领域插槽 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBEquipmentDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    /** 装备配置标识 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    FName EquipmentId;

    /** 装备显示名称 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    FText DisplayName;

    /** 装备说明 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    FText Description;

    /** 装备图标 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    TObjectPtr<UTexture2D> Icon;

    /**
     * 历史修复说明：旧版使用内嵌UObject保存固定Fragment，UE 5.6详情面板创建对象时会递归刷新并导致栈溢出
     * 当前插槽类型固定，不需要对象身份与多态，因此改为结构体值配置以避开该编辑器路径
     */

    /** 装备领域 */

    /**
     * 后续修订说明：固定结构体限制了Fragment替换能力，现由TInstancedStruct保存Domin派生Fragment
     * Domin统一行为契约，具体Fragment保留内联配置且不重新引入内嵌UObject
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fragment", meta = (ExcludeBaseStruct))
    TInstancedStruct<FBBBEquipDomin> EquipDomin;

    /** 开火领域 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fragment", meta = (ExcludeBaseStruct))
    TInstancedStruct<FBBBFireDomin> FireDomin;

    /** 弹匣领域 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fragment", meta = (ExcludeBaseStruct))
    TInstancedStruct<FBBBMagazineDomin> MagazineDomin;
};
