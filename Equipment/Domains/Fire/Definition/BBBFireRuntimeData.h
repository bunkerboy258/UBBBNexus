#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBFireRuntimeData.generated.h"

class UBBBSingleProjectileFireDomain;

/** 开火操作域运行数据 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBFireRuntimeData final : public UObject
{
    GENERATED_BODY()

private:
    friend class UBBBSingleProjectileFireDomain;

    /** 上次成功开火时间 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    float LastFireTime = -1000.0f;
};
