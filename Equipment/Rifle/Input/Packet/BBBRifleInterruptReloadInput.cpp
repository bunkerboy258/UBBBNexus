#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleInterruptReloadInput.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBReloadCancelledFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Animation/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Definition/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/DomainData/Context/BBBRifleInputContext.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/RuntimeData/BBBRifleRuntimeData.h"

bool FBBBRifleInterruptReloadInput::IsValid() const
{
    return true;
}

bool FBBBRifleInterruptReloadInput::CanApply(const FBBBRifleInputContext &Context) const
{
    const auto &State = Context.RuntimeData.Rifle.ReadRifleActionState();
    if (!State.bIsReloading)
    {
        return false;
    }

    return Sequence == INDEX_NONE || Sequence == State.ReloadSequence;
}

void FBBBRifleInterruptReloadInput::Apply(FBBBRifleInputContext &Context) const
{
    auto &State = Context.RuntimeData.Rifle.Action;
    State.bIsReloading = false;
    State.bMagazineDetached = false;
    UBBBEquipmentAnimInstance *AnimationInstance = Context.Equipment.GetEquipmentAnimationInstance();
    if (AnimationInstance && Context.Definition.EquipmentReloadMontage)
    {
        AnimationInstance->Montage_Stop(0.1f, Context.Definition.EquipmentReloadMontage);
    }

    Context.Character.SubmitInput(FBBBReloadCancelledFactPacket{
        Context.Equipment.GetEquipmentId(),
        State.ReloadSequence,
        State.LoadedAmmo});
}
