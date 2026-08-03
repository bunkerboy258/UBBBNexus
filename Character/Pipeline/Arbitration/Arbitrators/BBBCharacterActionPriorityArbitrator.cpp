
#include "BBBWork/UBBBNexus/Character/Pipeline/Arbitration/Arbitrators/BBBCharacterActionPriorityArbitrator.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Definition/BBBDecisionTypes.h"

void FBBBCharacterActionPriorityArbitrator::Update(
    FBBBDecisionRuntimeData &DecisionData) const
{
    TMap<EBBBCharacterActionDomain, int32> WinnerIndices;

    //每个动作域保留优先级最高的请求
    for (int32 Index = 0; Index < DecisionData.GetRequestCount(); ++Index)
    {
        const FBBBCharacterActionRequest &Request = DecisionData.GetRequest(Index);

        if (Request.GetArbitrationResult() != EBBBArbitrationResult::Pending)
        { continue; }

        int32 *WinnerIndex = WinnerIndices.Find(Request.GetDomain());

        if (!WinnerIndex)
        {
            WinnerIndices.Add(Request.GetDomain(), Index);

            continue;
        }

        if (Request.GetPriority() > DecisionData.GetRequest(*WinnerIndex).GetPriority())
        {
            WinnerIndices.Add(Request.GetDomain(), Index);
        }
    }

    for (int32 Index = 0; Index < DecisionData.GetRequestCount(); ++Index)
    {
        FBBBCharacterActionRequest &Request = DecisionData.AccessRequestForArbitration(Index);

        if (Request.GetArbitrationResult() != EBBBArbitrationResult::Pending)
        { continue; }

        if (WinnerIndices.FindRef(Request.GetDomain()) == Index)
        {
            Request.Approve();

            continue;
        }

        Request.RejectByPriority();
    }
}
