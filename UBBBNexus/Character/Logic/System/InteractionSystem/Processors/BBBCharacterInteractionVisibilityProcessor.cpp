#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/Processors/BBBCharacterInteractionVisibilityProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/Context/BBBCharacterInteractionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"

void FBBBCharacterInteractionVisibilityProcessor::Update(FBBBCharacterInteractionUpdateContext &Context) const
{
    if (Context.bIsMirror)
    {
        return;
    }

    auto &Selection = Context.RuntimeData.Interaction.InteractionSelectionState;
    auto &Inputs = Context.RuntimeData.Interaction.InteractionInputState;
    auto *Equipment = Cast<ABBBEquipment>(Selection.ActiveMainHandInstance);
    if (Selection.VisibilityGeneration != Selection.ActiveGeneration)
    {
        Selection.VisibilityGeneration = Selection.ActiveGeneration;
        Selection.bVisibilityForced = false;
    }
    const auto &Use = Context.RuntimeData.Interaction.ReadInteractionUseState();
    const bool bHideForRecovery = Use.bBusy && !Use.Fact.bQuickMelee &&
                                  IsValid(Use.Definition) && !Use.Definition->bThrowable;
    const auto &Life = Context.RuntimeData.Life.ReadLifeState();
    const auto &Rescue = Context.RuntimeData.Life.ReadRescueState();
    const auto &Physical = Context.RuntimeData.PhysicalPresentation.ReadPhysicalPresentationState();
    const bool bHideForLifeAction = !Physical.bRagdoll && !Physical.bRagdollRequested
        && (Life.Phase == EBBBCharacterLifePhase::Downed || Rescue.bHelping || Rescue.bReceiving);
    const bool bForced = bHideForLifeAction || bHideForRecovery;
    const bool bChanged = bForced != Selection.bVisibilityForced;
    Selection.bVisibilityForced = bForced;
    const bool bShow = Inputs.ShowEquipmentGenerations.Contains(Selection.ActiveGeneration);
    const bool bHide = Inputs.HideEquipmentGenerations.Contains(Selection.ActiveGeneration);
    Inputs.ShowEquipmentGenerations.Reset();
    Inputs.HideEquipmentGenerations.Reset();
    if (!Equipment)
    {
        return;
    }
    if (bHide || (bChanged && bForced))
    {
        Equipment->SubmitInput(FBBBEquipmentHideLocalControlPacket<>{});
    }
    if (!bForced && (bShow || (bChanged && !bHide)))
    {
        Equipment->SubmitInput(FBBBEquipmentShowLocalControlPacket<>{});
    }
}
