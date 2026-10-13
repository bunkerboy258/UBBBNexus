#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBBWork/UBBBNexus/Item/BBBItemDefinition.h"
#include "BBBMiscCatalog.generated.h"

/** 杂物的静态注册目录 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBMiscCatalog final : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    /** @param ItemId	型号标识 @return 唯一有效定义 */
    const UBBBItemDefinition *FindItem(FName ItemId) const;

    /** 杂物定义 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "物品列表"))
    TArray<TObjectPtr<UBBBItemDefinition>> Items;
};
