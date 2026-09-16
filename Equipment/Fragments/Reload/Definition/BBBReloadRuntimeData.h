#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBReloadRuntimeData.generated.h"

class UBBBEquipmentInstance;

/** 单件装备的换弹持续状态 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBReloadRuntimeData final : public UObject
{
    GENERATED_BODY()

public:
    /** @return 本件装备是否正在换弹 */
    bool IsReloading() const
    {
        return bIsReloading;
    }

    /** @return 换弹动作开始世界时间 */
    float GetStartTimeSeconds() const
    {
        return StartTimeSeconds;
    }

    /** @return 当前换弹动作持续时间 */
    float GetDurationSeconds() const
    {
        return DurationSeconds;
    }

    /** @return 最近一次换弹序号 */
    int32 GetSequence() const
    {
        return Sequence;
    }

private:
    friend class UBBBEquipmentInstance;

    UPROPERTY()
    bool bIsReloading = false;

    UPROPERTY()
    float StartTimeSeconds = 0.0f;

    UPROPERTY()
    float DurationSeconds = 0.0f;

    UPROPERTY()
    int32 Sequence = 0;
};
