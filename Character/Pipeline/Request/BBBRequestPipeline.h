
#pragma once
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Planners/BBBCharacterActionPlanner.h"
class FBBBCharacterInitializer;
struct FBBBDecisionRuntimeData;
struct FBBBIntentRuntimeData;

//把角色意图转换为动作请求
class FBBBRequestPipeline final
{
public:

    void Update() const;
    
private:
    
    friend class FBBBCharacterInitializer;

    void Initialize(
        FBBBDecisionRuntimeData &InDecisionData,
        const FBBBIntentRuntimeData &InIntentData);

    const FBBBIntentRuntimeData *IntentData = nullptr;

    FBBBDecisionRuntimeData *DecisionData = nullptr;

    FBBBCharacterActionPlanner ActionPlanner;
};
