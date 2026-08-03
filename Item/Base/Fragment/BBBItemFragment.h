
#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BBBItemFragment.generated.h"
class UBBBItemFragmentRuntimeData;

UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class ABBB_EVAC_API UBBBItemFragment : public UObject
{
    GENERATED_BODY()
public:

    virtual UBBBItemFragmentRuntimeData *InitializeRuntimeData(UObject &Outer) const
    {

        return nullptr;
    }
};
