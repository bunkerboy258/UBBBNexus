#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Config/Item/BBBItemDefinition.h"
#include "BBBWork/UBBBNexus/Character/Config/Appearance/BBBAppearanceResource.h"
#include "BBBItemCatalogEntry.generated.h"
class ABBBEquipment;

/** 统一物品目录中的一项静态注册 */
USTRUCT(BlueprintType)
struct FBBBItemCatalogEntry
{
    GENERATED_BODY()
    /** 物品型号定义 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "物品定义"))
    TObjectPtr<UBBBItemDefinition> Definition = nullptr;
    /** 只有装备条目允许配置的运行时实体类 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "装备实例类"))
    TSubclassOf<ABBBEquipment> EquipmentClass;
    /** 穿戴品或杂物提供的显示资源 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "外观资源"))
    FBBBAppearanceResource Appearance;
};
