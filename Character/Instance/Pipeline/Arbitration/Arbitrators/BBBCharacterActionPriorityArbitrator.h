
#pragma once
struct FBBBDecisionRuntimeData;

class FBBBCharacterActionPriorityArbitrator final
{
public:

    /**
     * 按动作域比较优先级批准最高请求并驳回其余
     * @param DecisionData	角色仲裁数据
     */
    void Update(FBBBDecisionRuntimeData &DecisionData) const;
};
