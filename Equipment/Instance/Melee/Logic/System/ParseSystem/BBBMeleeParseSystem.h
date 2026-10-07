#pragma once
struct FBBBMeleeUpdateContext;
/** 近战Parse领域固定调度根 */
class FBBBMeleeParseSystem final
{
public:
    /** @param Context 当前帧装配上下文 @return 无 */
    static void Update(FBBBMeleeUpdateContext &Context);
};
