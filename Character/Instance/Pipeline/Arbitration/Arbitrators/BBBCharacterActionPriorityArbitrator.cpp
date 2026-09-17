
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Arbitration/Arbitrators/BBBCharacterActionPriorityArbitrator.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"

void FBBBCharacterActionPriorityArbitrator::Update(
    FBBBDecisionRuntimeData &DecisionData) const
{
    // 为每个动作领域暂存当前优先级最高的请求
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
            // 领域首次出现请求时将其作为当前候选
            WinnerIndices.Add(Request.GetDomain(), Index);

            continue;
        }

        if (Request.GetPriority() > DecisionData.GetRequest(*WinnerIndex).GetPriority())
        {
            // 更高优先级请求替换同一领域的原候选
            WinnerIndices.Add(Request.GetDomain(), Index);
        }
    }

    // 第二次遍历将候选标记为批准并驳回其余请求
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
