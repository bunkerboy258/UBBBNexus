#pragma once
struct FBBBCharacterNetworkUpdateContext;

/** 只同步控制者已成立的翻越开始与结束结果 */
class FBBBTraversalObservationProcessor final
{
public:
    /** @param Context 本帧网络上下文 @return 无 */
    void Update(FBBBCharacterNetworkUpdateContext &Context) const;
};
