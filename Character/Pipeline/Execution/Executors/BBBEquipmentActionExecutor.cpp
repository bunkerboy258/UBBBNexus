
#include "BBBWork/UBBBNexus/Character/Pipeline/Execution/Executors/BBBEquipmentActionExecutor.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"

void FBBBEquipmentActionExecutor::Update(
    FBBBDecisionRuntimeData &DecisionData,
    FBBBCharacterEquipmentCommands &EquipmentCommands) const
{
    for (int32 Index = 0; Index < DecisionData.GetRequestCount(); ++Index)
    {
        FBBBCharacterActionRequest &Request = DecisionData.AccessRequestForExecution(Index);

        if (Request.GetArbitrationResult() != EBBBArbitrationResult::Approved)
        { continue; }

        if (Request.GetType() == EBBBCharacterActionType::Fire)
        {
            EquipmentCommands.SubmitFire();
            Request.MarkConsumed();
        }

        if (Request.GetType() == EBBBCharacterActionType::Reload)
        {
            EquipmentCommands.SubmitReload();
            Request.MarkConsumed();
        }
    }
}
