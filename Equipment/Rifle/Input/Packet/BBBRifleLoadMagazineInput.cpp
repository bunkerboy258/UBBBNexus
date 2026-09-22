#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleLoadMagazineInput.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBMagazineLoadedFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/BBBRifleInputContext.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/RuntimeData/BBBRifleRuntimeData.h"

bool FBBBRifleLoadMagazineInput::IsValid() const
{
    return true;
}

bool FBBBRifleLoadMagazineInput::CanApply(const FBBBRifleInputContext &Context) const
{
    if (!Context.RuntimeData.bIsReloading || !Context.RuntimeData.bMagazineDetached)
    {
        return false;
    }

    return Sequence == INDEX_NONE || Sequence == Context.RuntimeData.ReloadSequence;
}

void FBBBRifleLoadMagazineInput::Apply(FBBBRifleInputContext &Context) const
{
    Context.RuntimeData.LoadedAmmo = Context.RuntimeData.AmmoCapacity;
    Context.RuntimeData.bIsReloading = false;
    Context.RuntimeData.bMagazineDetached = false;
    Context.Character.SubmitInput(FBBBMagazineLoadedFactPacket{
        Context.Equipment.GetEquipmentId(),
        Context.RuntimeData.ReloadSequence,
        Context.RuntimeData.LoadedAmmo});
}
