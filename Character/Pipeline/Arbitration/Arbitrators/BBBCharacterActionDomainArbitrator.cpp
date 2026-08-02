
#include "BBBWork/UBBBNexus/Character/Pipeline/Arbitration/Arbitrators/BBBCharacterActionDomainArbitrator.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Definition/BBBDecisionTypes.h"

void FBBBCharacterActionDomainArbitrator::Update(
    FBBBDecisionRuntimeData &DecisionData) const
{
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
    {
        return;
    }

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
