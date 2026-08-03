
#pragma once
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Planners/BBBCharacterActionPlanner.h"
class FBBBCharacterInitializer;
struct FBBBDecisionRuntimeData;
struct FBBBIntentRuntimeData;

//把角色意图转换为动作请求
class FBBBRequestPipeline final
{
public:

    /**
     * 把当前帧意图转换为动作请求
     */
    void Update() const;
    
private:
    
    friend class FBBBCharacterInitializer;

    /**
     * 注入请求管线固定依赖
     * @param InDecisionData	角色仲裁数据
     * @param InIntentData	当前帧行为意图
     */
    void Initialize(
        FBBBDecisionRuntimeData &InDecisionData,
        const FBBBIntentRuntimeData &InIntentData);

    const FBBBIntentRuntimeData *IntentData = nullptr;

    FBBBDecisionRuntimeData *DecisionData = nullptr;

    FBBBCharacterActionPlanner ActionPlanner;
};
