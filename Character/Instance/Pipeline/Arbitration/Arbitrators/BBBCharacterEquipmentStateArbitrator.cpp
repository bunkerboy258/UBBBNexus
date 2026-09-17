#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Arbitration/Arbitrators/BBBCharacterEquipmentStateArbitrator.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"

void FBBBCharacterEquipmentStateArbitrator::Update(
    FBBBDecisionRuntimeData &DecisionData, const FBBBCharacterEquipmentState &EquipmentState) const
{
    // 先清理不再对应当前换弹状态的动画通知
    DecisionData.ReloadInputs.RemoveAll([&EquipmentState](const FBBBCharacterReloadAnimationInput &Input)
    {
        const bool bInvalid = !EquipmentState.GetActiveMainHandInstance()
            || !EquipmentState.IsReloading() || Input.Sequence != EquipmentState.GetReloadSequence();
        if (bInvalid)
        {
            UE_LOG(LogTemp, Verbose, TEXT("[UBBBC]Ignored stale reload animation input Sequence=%d"), Input.Sequence);
        }
        return bInvalid;
    });

    if (!EquipmentState.IsReloading())
    {
        // 非换弹状态不需要限制开火和换弹请求
        return;
    }

    // 换弹期间只处理尚未被其他规则决定的请求
    for (int32 Index = 0; Index < DecisionData.GetRequestCount(); ++Index)
    {
        FBBBCharacterActionRequest &Request = DecisionData.AccessRequestForArbitration(Index);
        if (Request.GetArbitrationResult() != EBBBArbitrationResult::Pending)
        {
            continue;
        }
        // 换弹占用装备动作领域期间拒绝新的开火和换弹请求
        if (Request.GetType() == EBBBCharacterActionType::Fire || Request.GetType() == EBBBCharacterActionType::Reload)
        {
            Request.RejectByDomain();
        }
    }
}
