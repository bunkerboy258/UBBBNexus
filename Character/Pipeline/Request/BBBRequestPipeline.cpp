
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/BBBRequestPipeline.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Definition/BBBDecisionTypes.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"

void FBBBRequestPipeline::Initialize(
    FBBBDecisionRuntimeData &InDecisionData,
    const FBBBIntentRuntimeData &InIntentData)
{
    DecisionData = &InDecisionData;
    IntentData = &InIntentData;
}

void FBBBRequestPipeline::Update() const
{
    if (!ensureMsgf(IntentData && DecisionData, TEXT("[UBBBC]Request pipeline update failed because dependencies are null")))
    {
        return;
    }

    ActionPlanner.Update(*IntentData, *DecisionData);
}
