#pragma once
#include "CoreMinimal.h"
#include "BBBEquipmentAmmoConfig.generated.h"

/** 最小弹药配置，装填时补满容量 */
USTRUCT(BlueprintType)
struct FBBBEquipmentAmmoConfig
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1"))
    int32 AmmoCapacity = 30;
};
