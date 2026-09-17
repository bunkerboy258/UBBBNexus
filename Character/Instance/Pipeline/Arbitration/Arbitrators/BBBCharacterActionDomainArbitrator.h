
#pragma once
struct FBBBDecisionRuntimeData;

class FBBBCharacterActionDomainArbitrator final
{
public:

    /**
     * 存在全身域待决请求时驳回所有上半身域请求
     * @param DecisionData	角色仲裁数据
     */
    void Update(FBBBDecisionRuntimeData &DecisionData) const;
};
