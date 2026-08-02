
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/RuntimeData/BBBItemFragmentRuntimeData.h"
#include "BBBMagazineRuntimeData.generated.h"

UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBMagazineRuntimeData final : public UBBBItemFragmentRuntimeData
{
    GENERATED_BODY()
public:

    UPROPERTY(BlueprintReadOnly)
    int32 MagazineAmmo = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 ReserveAmmo = 0;

    UPROPERTY(BlueprintReadOnly)
    bool bIsReloading = false;

    bool bCommitReloadAmmo = false;

    float ReloadEndTime = 0.0f;
};
