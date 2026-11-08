#pragma once

#include "Animation/AnimSequence.h"
#include "BBBMonsterTraversalClip.generated.h"

/** 一个高度段的上台动作与接触窗口 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterTraversalClip
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "攀爬", meta = (DisplayName = "动作序列"))
    TObjectPtr<UAnimSequence> Sequence;

    UPROPERTY(EditAnywhere, Category = "攀爬", meta = (DisplayName = "抓边开始进度", ClampMin = "0", ClampMax = "1"))
    float ContactStart = 0.2f;

    UPROPERTY(EditAnywhere, Category = "攀爬", meta = (DisplayName = "抓边结束进度", ClampMin = "0", ClampMax = "1"))
    float ContactEnd = 0.65f;

    UPROPERTY(EditAnywhere, Category = "攀爬", meta = (DisplayName = "手掌间距", Units = "cm", ClampMin = "15", ClampMax = "70"))
    float HandSpacing = 42.0f;
};
