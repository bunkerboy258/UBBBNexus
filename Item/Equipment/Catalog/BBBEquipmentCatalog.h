#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Catalog/BBBEquipmentCatalogEntry.h"
#include "BBBEquipmentCatalog.generated.h"

/** 本类别的唯一静态注册目录 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBEquipmentCatalog final : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    /** @param ItemId	型号标识 @return 唯一有效条目 */
    const FBBBEquipmentCatalogEntry *FindItem(FName ItemId) const;

    /** 已注册的物品 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "物品列表"))
    TArray<FBBBEquipmentCatalogEntry> Items;
};
