#pragma once

struct FBBBRifleUpdateContext;

/** 步枪Parse系统的固定调度入口 */
class FBBBRifleParseSystem final
{
public:
    /** @param Context	本次装备更新上下文 @return 无 */
    static void Update(FBBBRifleUpdateContext &Context);
};
