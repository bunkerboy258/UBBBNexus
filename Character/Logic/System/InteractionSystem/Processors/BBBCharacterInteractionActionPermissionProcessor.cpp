#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/Processors/BBBCharacterInteractionActionPermissionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/Context/BBBCharacterInteractionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"

void FBBBCharacterInteractionActionPermissionProcessor::Update(FBBBCharacterInteractionUpdateContext &Context) const
{
    auto &Use = Context.RuntimeData.Interaction.InteractionUseState;
    AActor *Item = Context.RuntimeData.Interaction.ReadInteractionSelectionState().ActiveMainHandInstance;
    auto *Equipment = Cast<ABBBEquipment>(Item);
    auto *Consumable = Cast<ABBBConsumable>(Item);
    Use.UseSerial = Consumable ? Consumable->GetUseSerial() : 0;
    Use.bBusy = Consumable && Consumable->IsUsing();
    Use.bSwitchAllowed = !Use.bBusy && (!Equipment || Equipment->CanSwitch());
    Use.bQuickUseAllowed = Use.bSwitchAllowed && (!Equipment || Equipment->CanQuickUse());
}
