#pragma once
struct FBBBCharacterNetworkUpdateContext;

/** 经合法拥有的角色连接把独立命中投送给控制者 */
class FBBBCharacterDamageObservationProcessor final
{
  public:
    /**
     * @param Context	本次只读事实与所属领域上下文
     * @return 无
     */
    void Update(FBBBCharacterNetworkUpdateContext &Context) const;
};
