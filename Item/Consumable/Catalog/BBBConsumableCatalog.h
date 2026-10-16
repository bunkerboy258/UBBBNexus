#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBBConsumableCatalog.generated.h"
class UBBBConsumableDefinition;
/** 仅登记消耗品静态定义的型号目录 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBConsumableCatalog final : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
  /** @param ItemId 型号标识 @return 唯一有效定义 */
  UBBBConsumableDefinition *FindItem(FName ItemId) const;
  /** 型号定义资产 */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "型号定义"))
  TArray<TObjectPtr<UBBBConsumableDefinition>> Items;
};
