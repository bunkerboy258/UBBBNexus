
#include "BBBWork/UBBBNexus/Character/Pipeline/Arbitration/BBBArbitrationPipeline.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Definition/BBBDecisionTypes.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Context/BBBCharacterItemStates.h"

void FBBBArbitrationPipeline::Initialize(
    FBBBDecisionRuntimeData &InDecisionData,
    const FBBBCharacterEquipmentState &InEquipmentState)
{
    DecisionData = &InDecisionData;
    EquipmentState = &InEquipmentState;
}

void FBBBArbitrationPipeline::Update() const
{
    if (!ensureMsgf(DecisionData && EquipmentState, TEXT("[UBBBC]Arbitration pipeline update failed because dependencies are null")))
    { return; }

    EquipmentStateArbitrator.Update(*DecisionData, *EquipmentState);
    ActionDomainArbitrator.Update(*DecisionData);
    ActionPriorityArbitrator.Update(*DecisionData);
}
