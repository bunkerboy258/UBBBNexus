#pragma once

#include "CoreMinimal.h"
#include "BBBAimDomin.generated.h"

class UAnimSequence;

/**
 * 装备瞄准操作域基类
 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBAimDomin
{
    GENERATED_BODY()

public:
    virtual ~FBBBAimDomin() = default;

    /** @return 瞄准期间使用的上半身动画 */
    virtual UAnimSequence *GetUpperBodyAnimation() const;
};
