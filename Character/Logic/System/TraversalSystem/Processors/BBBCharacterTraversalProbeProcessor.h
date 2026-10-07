#pragma once

struct FBBBCharacterTraversalUpdateContext;

/** 只在跳跃输入时探测可到达的翻越目标 */
class FBBBCharacterTraversalProbeProcessor final
{
public:
    /** @param Context 本帧移动上下文 @return 无 */
    void Update(FBBBCharacterTraversalUpdateContext &Context) const;
};
