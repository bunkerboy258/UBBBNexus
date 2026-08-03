
#pragma once
#include "CoreMinimal.h"
struct FBBBDecisionRuntimeData;
struct FBBBIntentRuntimeData;

class ABBB_EVAC_API FBBBCharacterActionPlanner final
{
public:

    /**
     * 把行为意图按动作域与优先级转换为动作请求
     * @param IntentData	当前帧行为意图
     * @param DecisionData	角色仲裁数据
     */
    void Update(
        const FBBBIntentRuntimeData &IntentData,
        FBBBDecisionRuntimeData &DecisionData) const;
};
