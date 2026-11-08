#pragma once

struct FBBBCharacterPhysicalPresentationUpdateContext;

/** 消费本机已成立的冲击并请求现有物理接管与安全起身 */
class FBBBCharacterKnockdownProcessor final
{
public:
    /** @param Context	本帧角色与物理依赖 @return 无 */
    void Update(FBBBCharacterPhysicalPresentationUpdateContext &Context) const;
};
