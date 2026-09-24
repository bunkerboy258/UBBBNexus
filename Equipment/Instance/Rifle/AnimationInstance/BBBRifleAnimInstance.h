#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Animation/BBBEquipmentAnimInstance.h"
#include "BBBRifleAnimInstance.generated.h"

struct FBBBRifleActionState;

/** 步枪动画图只读的动作结果 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBRifleAnimInstance : public UBBBEquipmentAnimInstance
{
    GENERATED_BODY()

public:
    /** @param State	本帧动作结果 @param WorldTime	本机世界时间 @return 无 */
    void PublishRifleState(const FBBBRifleActionState &State, float WorldTime);

    /** @return 步枪是否正在换弹 */
    UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
    bool IsReloading() const
    {
        return bIsReloading;
    }

    /** @return 最近一次开火至当前快照的间隔 */
    UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
    float GetTimeSinceLastFireSeconds() const
    {
        return TimeSinceLastFireSeconds;
    }

protected:
    /** 当前弹量 */
    UPROPERTY(BlueprintReadOnly, Transient)
    int32 LoadedAmmo = 0;

    /** 当前弹匣容量 */
    UPROPERTY(BlueprintReadOnly, Transient)
    int32 AmmoCapacity = 0;

    /** 当前换弹状态 */
    UPROPERTY(BlueprintReadOnly, Transient)
    bool bIsReloading = false;

    /** 当前快照距离最近开火的时间 */
    UPROPERTY(BlueprintReadOnly, Transient)
    float TimeSinceLastFireSeconds = 0.0f;
};
