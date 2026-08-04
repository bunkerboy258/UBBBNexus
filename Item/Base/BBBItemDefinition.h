
#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBBItemDefinition.generated.h"
class UTexture2D;

UCLASS(Abstract, BlueprintType)
class ABBB_EVAC_API UBBBItemDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Item")
    FName ItemId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Item")
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Item")
    FText Description;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Item")
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Item", meta = (ClampMin = "1"))
    int32 MaxStack = 1;
};
