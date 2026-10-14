#pragma once
#include "CoreMinimal.h"
struct FBBBMeleeAnimationUpdateContext;
struct FBBBMeleeRuntimeData;
/** 近战Animation处理器 */
class FBBBMeleeAnimationProcessor final
{
public:
    /** @param Context	本帧装配上下文 @return 无 */
    static void Update(FBBBMeleeAnimationUpdateContext &Context);
    /** @param Data	当前装备黑板 @return 无 */
    static void Stop(FBBBMeleeAnimationUpdateContext &Context);
};
