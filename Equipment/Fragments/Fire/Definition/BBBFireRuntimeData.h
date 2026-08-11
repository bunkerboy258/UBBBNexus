#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBFireRuntimeData.generated.h"

struct FBBBSingleProjectileFireFragment;

/** 开火操作域运行数据 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBFireRuntimeData final : public UObject
{
    GENERATED_BODY()

private:
    friend struct FBBBSingleProjectileFireFragment;

    /** 上次成功开火时间 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    float LastFireTime = -1000.0f;

    /** 是否正在播放开火动作 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    bool bIsFiring = false;

    /** 开火动作结束时间 */
    float FireEndTime = 0.0f;
};
