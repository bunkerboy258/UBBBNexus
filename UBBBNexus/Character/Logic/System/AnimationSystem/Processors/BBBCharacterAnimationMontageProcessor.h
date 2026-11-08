#pragma once

#include "CoreMinimal.h"

struct FBBBCharacterAnimationUpdateContext;

/** 消费动画系统固定槽位的蒙太奇请求 */
class ABBB_EVAC_API FBBBCharacterAnimationMontageProcessor final
{
public:
    /**
     * 消费每个固定槽位的单次蒙太奇请求
     * @param Context	本次动画更新上下文
     * @return 无
     */
    void Update(FBBBCharacterAnimationUpdateContext &Context) const;
};
