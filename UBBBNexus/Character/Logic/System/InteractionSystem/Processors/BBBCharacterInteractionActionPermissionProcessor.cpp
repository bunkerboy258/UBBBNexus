#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/Processors/BBBCharacterInteractionActionPermissionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/Context/BBBCharacterInteractionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
void FBBBCharacterInteractionActionPermissionProcessor::Update(FBBBCharacterInteractionUpdateContext &Context) const
{
    auto &Use = Context.RuntimeData.Interaction.InteractionUseState;
    auto *Equipment = Context.Character.GetHeldEquipment();
    Use.bSwitchAllowed = !Use.bBusy && (!Equipment || Equipment->CanSwitch());
    Use.bQuickUseAllowed = Use.bSwitchAllowed && (!Equipment || Equipment->CanQuickUse());
}
