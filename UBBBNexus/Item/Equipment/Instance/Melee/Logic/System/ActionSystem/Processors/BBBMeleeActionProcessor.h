#pragma once
#include "CoreMinimal.h"
struct FBBBMeleeActionUpdateContext;
struct FBBBMeleeRuntimeData;
/** 近战Action处理器 */
class FBBBMeleeActionProcessor final
{
public:
    /** @param Context	本帧装配上下文 @return 无 */
    static void Update(FBBBMeleeActionUpdateContext &Context);
    /** @param Data	当前装备黑板 @return 无 */
    static void Stop(FBBBMeleeRuntimeData &Data);
};
