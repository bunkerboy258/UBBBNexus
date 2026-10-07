#pragma once
#include "CoreMinimal.h"
struct FBBBMeleeUpdateContext;
struct FBBBMeleeRuntimeData;
/** 近战Contact处理器 */
class FBBBMeleeContactProcessor final
{
public:
    /** @param Context	本帧装配上下文 @return 无 */
    static void Update(FBBBMeleeUpdateContext &Context);
};
