#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleInterruptReloadInput.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBReloadCancelledFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/BBBRifleInputContext.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/RuntimeData/BBBRifleRuntimeData.h"

bool FBBBRifleInterruptReloadInput::IsValid() const
{
    return true;
}

bool FBBBRifleInterruptReloadInput::CanApply(const FBBBRifleInputContext &Context) const
{
    if (!Context.RuntimeData.bIsReloading)
    {
        return false;
    }

    return Sequence == INDEX_NONE || Sequence == Context.RuntimeData.ReloadSequence;
}

void FBBBRifleInterruptReloadInput::Apply(FBBBRifleInputContext &Context) const
{
    Context.RuntimeData.bIsReloading = false;
    Context.RuntimeData.bMagazineDetached = false;
    Context.Character.SubmitInput(FBBBReloadCancelledFactPacket{
        Context.Equipment.GetEquipmentId(),
        Context.RuntimeData.ReloadSequence,
        Context.RuntimeData.LoadedAmmo});
}
