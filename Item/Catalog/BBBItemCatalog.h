#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBBWork/UBBBNexus/Item/Catalog/BBBItemCatalogEntry.h"
#include "BBBItemCatalog.generated.h"

/** 武器 穿戴品与杂物共享的外侧注册目录 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBItemCatalog final : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    /** @param ItemId 物品型号 @return 唯一有效条目 无效或重复时为空 */
    const FBBBItemCatalogEntry *FindItem(FName ItemId) const;
    /** @param ItemId 装备型号 @return 有效装备类 其它物品为空 */
    TSubclassOf<ABBBEquipment> FindEquipmentClass(FName ItemId) const;
    /** 全部真实物品注册 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "物品列表"))
    TArray<FBBBItemCatalogEntry> Items;
};
