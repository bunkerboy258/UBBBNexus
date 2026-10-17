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

    const auto &Use = Context.RuntimeData.Interaction.ReadInteractionUseState();
    const bool bHideForRecovery = Use.bBusy && !Use.Fact.bQuickMelee &&
                                  IsValid(Use.Definition) && !Use.Definition->bThrowable;
    const bool bHide = Context.RuntimeData.Traversal.ReadTraversalState().bTraversing ||
                       !Context.RuntimeData.Life.ReadLifeState().bActionsAllowed ||
                       bHideForRecovery;
    AActor *Item = Context.RuntimeData.Interaction.ReadInteractionSelectionState().ActiveMainHandInstance;
    if (auto *Equipment = Cast<ABBBEquipment>(Item); Equipment && Equipment->IsMeshHidden() != bHide)
    {
        if (bHide)
        {
            Equipment->SubmitInput(FBBBEquipmentHideLocalControlPacket<>{});
        }
        else
        {
            Equipment->SubmitInput(FBBBEquipmentShowLocalControlPacket<>{});
        }
    }
}
