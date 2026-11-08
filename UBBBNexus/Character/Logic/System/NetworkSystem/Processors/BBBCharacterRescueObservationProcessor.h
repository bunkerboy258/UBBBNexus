#pragma once

struct FBBBCharacterNetworkUpdateContext;

/** 观察救援既成结果并通过所属连接分发 */
class FBBBCharacterRescueObservationProcessor final
{
public:
    /** @param Context	本次网络上下文 @return 无 */
    void Update(FBBBCharacterNetworkUpdateContext &Context) const;
};
