#pragma once
#include "CoreMinimal.h"
#include "BBBConsumableNetworkMessage.generated.h"

/** 消耗品当前使用结果 不保存历史行为 */
USTRUCT()
struct FBBBConsumableNetworkMessage final
{
    GENERATED_BODY()
    /** 结果对应的物品型号 */
    UPROPERTY()
    FName ItemId;
    /** 持有实例代次 */
    UPROPERTY()
    uint64 Generation = 0;
    /** 当前结果的顺序 */
    UPROPERTY()
    uint64 Revision = 0;
    /** 已开始的使用序号 */
    UPROPERTY()
    int32 UseSerial = 0;
    /** 已完成的释放序号 */
    UPROPERTY()
    int32 ReleaseSerial = 0;
    /** 当前是否处于使用动作 */
    UPROPERTY()
    bool bUsing = false;
    /** 当前手持模型是否已释放 */
    UPROPERTY()
    bool bReleased = false;
    /** 最近一次已成立的释放变换 */
    UPROPERTY()
    FTransform LaunchTransform = FTransform::Identity;

    /** @return 结果结构是否有效 */
    bool IsValid() const
    {
        return !ItemId.IsNone() && Generation > 0 && Revision > 0 && UseSerial >= 0
            && ReleaseSerial >= 0 && ReleaseSerial <= UseSerial && !LaunchTransform.ContainsNaN();
    }
};
