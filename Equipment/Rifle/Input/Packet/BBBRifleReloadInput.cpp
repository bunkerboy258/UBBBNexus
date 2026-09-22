#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleReloadInput.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBReloadStartedFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Definition/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/DomainData/Context/BBBRifleInputContext.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Processors/BBBRiflePresentationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/RuntimeData/BBBRifleRuntimeData.h"

bool FBBBRifleReloadInput::IsValid() const
{
    return Sequence != INDEX_NONE;
}

bool FBBBRifleReloadInput::CanApply(const FBBBRifleInputContext &Context) const
{
    const auto &State = Context.RuntimeData.Rifle.ReadRifleActionState();
    return Context.Definition.CharacterReloadMontage
        && Context.Definition.EquipmentReloadMontage
        && !State.bIsReloading
        && State.LoadedAmmo < State.AmmoCapacity;
}

void FBBBRifleReloadInput::Apply(FBBBRifleInputContext &Context) const
{
    auto &State = Context.RuntimeData.Rifle.Action;
    FBBBRiflePresentationProcessor::SubmitCharacterMontage(
        Context,
        Context.Definition.CharacterReloadMontage,
        Sequence,
        true);
    FBBBRiflePresentationProcessor::PlayEquipmentMontage(Context, Context.Definition.EquipmentReloadMontage);

    State.bIsReloading = true;
    State.bMagazineDetached = false;
    State.ReloadSequence = Sequence;
    Context.Character.SubmitInput(FBBBReloadStartedFactPacket{
        Context.Equipment.GetEquipmentId(),
        Sequence,
        State.LoadedAmmo});
}
