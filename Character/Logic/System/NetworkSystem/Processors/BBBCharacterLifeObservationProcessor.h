#pragma once
struct FBBBCharacterNetworkUpdateContext;

/** 只观察并发送已成立的生命结果 */
class FBBBCharacterLifeObservationProcessor final
{
  public:
    /**
     * @param Context	本次只读事实与所属领域上下文
     * @return 无
     */
    void Update(FBBBCharacterNetworkUpdateContext &Context) const;
};
