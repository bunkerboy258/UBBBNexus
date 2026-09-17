
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Request/BBBRequestPipeline.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"

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
    { return; }

    ActionPlanner.Update(*IntentData, *DecisionData);
    DecisionData->ReloadInputs = IntentData->ReloadInputs;
}
