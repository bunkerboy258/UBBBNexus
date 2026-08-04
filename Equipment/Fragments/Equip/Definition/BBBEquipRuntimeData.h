#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBEquipRuntimeData.generated.h"

class UBBBEquipFragment;

/** 装备过渡运行数据 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBEquipRuntimeData final : public UObject
{
    GENERATED_BODY()

public:
    /** @return 是否处于装备过渡 */
    bool IsEquipping() const
    {
        return bIsEquipping;
    }

private:
    friend class UBBBEquipFragment;

    /** 是否处于装备过渡 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    bool bIsEquipping = false;

    /** 装备过渡结束时间 */
    float EquipEndTime = 0.0f;
};
