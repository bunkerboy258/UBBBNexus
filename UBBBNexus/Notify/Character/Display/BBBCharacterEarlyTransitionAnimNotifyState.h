#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BBBCharacterEarlyTransitionAnimNotifyState.generated.h"

/** 动画标记 当前片段允许提前交接到下一个姿势 */
UCLASS(meta = (DisplayName = "Early Transition"))
class UBBBCharacterEarlyTransitionAnimNotifyState final : public UAnimNotifyState
{
    GENERATED_BODY()
};
