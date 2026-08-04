#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBMagazineRuntimeData.generated.h"

class UBBBMagazineDomain;

/** 弹匣操作域运行数据 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBMagazineRuntimeData final : public UObject
{
    GENERATED_BODY()

private:
    friend class UBBBMagazineDomain;

    /** 当前弹匣弹药 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    int32 MagazineAmmo = 0;

    /** 当前备用弹药 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    int32 ReserveAmmo = 0;

    /** 是否正在换弹 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    bool bIsReloading = false;

    /** 换弹结束时间 */
    float ReloadEndTime = 0.0f;
};
