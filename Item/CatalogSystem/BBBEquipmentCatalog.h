#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBBEquipmentCatalog.generated.h"

class UBBBEquipmentDefinition;

UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBEquipmentCatalog final : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:

    UBBBEquipmentDefinition *FindDefinition(FName EquipmentHandle);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    TArray<TObjectPtr<UBBBEquipmentDefinition>> Definitions;
};
