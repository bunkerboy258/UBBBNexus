#pragma once
struct FBBBCharacterNetworkUpdateContext;
/** 观察并投送当前物理结果 不生成物理玩法 */
class FBBBCharacterRagdollObservationProcessor final
{
public:
    /** @param Context 网络更新上下文 @return 无 */
    void Update(FBBBCharacterNetworkUpdateContext &Context) const;
};
