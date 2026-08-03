
#include "BBBWork/UBBBNexus/Character/Pipeline/Execution/Executors/BBBItemActionExecutor.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/Commands/BBBCharacterItemCommands.h"

void FBBBItemActionExecutor::Update(
    FBBBDecisionRuntimeData &DecisionData,
    FBBBCharacterItemCommands &ItemCommands) const
{
    for (int32 Index = 0; Index < DecisionData.GetRequestCount(); ++Index)
    {
        FBBBCharacterActionRequest &Request = DecisionData.AccessRequestForExecution(Index);

        if (Request.GetArbitrationResult() != EBBBArbitrationResult::Approved)
        { continue; }

        if (Request.GetType() == EBBBCharacterActionType::Fire)
        {
            ItemCommands.SubmitFire();
            Request.MarkConsumed();
        }

        if (Request.GetType() == EBBBCharacterActionType::Reload)
        {
            ItemCommands.SubmitReload();
            Request.MarkConsumed();
        }
    }
}
