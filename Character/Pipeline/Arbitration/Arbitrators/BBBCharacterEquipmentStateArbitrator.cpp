
#include "BBBWork/UBBBNexus/Character/Pipeline/Arbitration/Arbitrators/BBBCharacterEquipmentStateArbitrator.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Definition/BBBDecisionTypes.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemState.h"

void FBBBCharacterEquipmentStateArbitrator::Update(
    FBBBDecisionRuntimeData &DecisionData,
    const FBBBCharacterEquipmentState &EquipmentState) const
{
    //装备稳定时不限制
    if (!EquipmentState.IsEquipping())
    { return; }

    for (int32 Index = 0; Index < DecisionData.GetRequestCount(); ++Index)
    {
        FBBBCharacterActionRequest &Request = DecisionData.AccessRequestForArbitration(Index);

        //跳过已被处理的请求
        if (Request.GetArbitrationResult() != EBBBArbitrationResult::Pending)
        { continue; }

        //拒绝所有占用身体的请求
        if (Request.GetDomain() != EBBBCharacterActionDomain::FullBody
            && Request.GetDomain() != EBBBCharacterActionDomain::UpperBody)
        { continue; }
        
        Request.RejectByDomain();
    }
}
