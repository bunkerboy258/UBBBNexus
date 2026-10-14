#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBBConsumableCatalog.generated.h"
class ABBBConsumable;

/** 只登记具体实例类的静态型号目录 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBConsumableCatalog final : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    /** @param ItemId 型号标识 @return 唯一有效实例类 */
    TSubclassOf<ABBBConsumable> FindItem(FName ItemId) const;

    /** 静态定义由所登记类的默认对象提供 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "型号实例类"))
    TArray<TSubclassOf<ABBBConsumable>> Items;
};
