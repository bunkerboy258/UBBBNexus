#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleDetachMagazineInput.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBMagazineDetachedFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/BBBRifleInputContext.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/RuntimeData/BBBRifleRuntimeData.h"

bool FBBBRifleDetachMagazineInput::IsValid() const
{
    return true;
}

bool FBBBRifleDetachMagazineInput::CanApply(const FBBBRifleInputContext &Context) const
{
    if (!Context.RuntimeData.bIsReloading || Context.RuntimeData.bMagazineDetached)
    {
        return false;
    }

    return Sequence == INDEX_NONE || Sequence == Context.RuntimeData.ReloadSequence;
}

void FBBBRifleDetachMagazineInput::Apply(FBBBRifleInputContext &Context) const
{
    Context.RuntimeData.LoadedAmmo = 0;
    Context.RuntimeData.bMagazineDetached = true;
    Context.Character.SubmitInput(FBBBMagazineDetachedFactPacket{
        Context.Equipment.GetEquipmentId(),
        Context.RuntimeData.ReloadSequence,
        Context.RuntimeData.LoadedAmmo});
}
