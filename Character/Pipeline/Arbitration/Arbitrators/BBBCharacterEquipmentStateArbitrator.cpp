#include "BBBWork/UBBBNexus/Character/Pipeline/Arbitration/Arbitrators/BBBCharacterEquipmentStateArbitrator.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"

void FBBBCharacterEquipmentStateArbitrator::Update(
    FBBBDecisionRuntimeData &DecisionData, const FBBBCharacterEquipmentState &EquipmentState) const
{
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
        return;
    }

    for (int32 Index = 0; Index < DecisionData.GetRequestCount(); ++Index)
    {
        FBBBCharacterActionRequest &Request = DecisionData.AccessRequestForArbitration(Index);
        if (Request.GetArbitrationResult() != EBBBArbitrationResult::Pending)
        {
            continue;
        }
        if (Request.GetType() == EBBBCharacterActionType::Fire || Request.GetType() == EBBBCharacterActionType::Reload)
        {
            Request.RejectByDomain();
        }
    }
}
