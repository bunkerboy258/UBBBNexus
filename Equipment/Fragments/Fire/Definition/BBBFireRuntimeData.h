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

public:
    /** @return 上次本地成功开火或远端开火表现的世界时间 */
    float GetLastFireTime() const
    {
        return LastFireTime;
    }

private:
    friend struct FBBBSingleProjectileFireFragment;

    /** 上次成功开火时间 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    float LastFireTime = -1000.0f;

};
