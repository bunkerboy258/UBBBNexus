#pragma once
struct FBBBMeleeUpdateContext;
/** 近战Action领域固定调度根 */
class FBBBMeleeActionSystem final
{
public:
    /** @param Context 当前帧装配上下文 @return 无 */
    static void Update(FBBBMeleeUpdateContext &Context);
};
