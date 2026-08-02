
#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBItemRuntimeData.generated.h"
class UBBBItemDefinition;
class UBBBItemFragmentRuntimeData;

UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBItemRuntimeData final : public UObject
{
    GENERATED_BODY()
public:

    void Initialize(const UBBBItemDefinition &Definition);

    template <typename T>
    T *FindRuntimeData() const
    {

        for (UBBBItemFragmentRuntimeData *RuntimeData : RuntimeDataList)
        {

            if (T *TypedRuntimeData = Cast<T>(RuntimeData))
            {
                return TypedRuntimeData;
            }
        }

        return nullptr;
    }
private:

    UPROPERTY(Instanced)
    TArray<TObjectPtr<UBBBItemFragmentRuntimeData>> RuntimeDataList;
};
