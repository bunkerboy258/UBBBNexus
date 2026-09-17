
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Arbitration/Arbitrators/BBBCharacterActionDomainArbitrator.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"

void FBBBCharacterActionDomainArbitrator::Update(
    FBBBDecisionRuntimeData &DecisionData) const
{
    // 先确认当前是否存在等待仲裁的全身动作
    bool bHasFullBody = false;

    for (int32 Index = 0; Index < DecisionData.GetRequestCount(); ++Index)
    {
        const FBBBCharacterActionRequest &Request = DecisionData.GetRequest(Index);

        if (Request.GetArbitrationResult() == EBBBArbitrationResult::Pending
            && Request.GetDomain() == EBBBCharacterActionDomain::FullBody)
        {
            bHasFullBody = true;
            break;
        }
    }

    if (!bHasFullBody)
    { return; }

    // 全身动作存在时驳回仍未决定的上半身动作
    //如果存在全身域的动作请求 废掉所有上半身动作
    for (int32 Index = 0; Index < DecisionData.GetRequestCount(); ++Index)
    {
        FBBBCharacterActionRequest &Request = DecisionData.AccessRequestForArbitration(Index);

        if (Request.GetArbitrationResult() == EBBBArbitrationResult::Pending
            && Request.GetDomain() == EBBBCharacterActionDomain::UpperBody)
        {
            Request.RejectByDomain();
        }
    }
}
