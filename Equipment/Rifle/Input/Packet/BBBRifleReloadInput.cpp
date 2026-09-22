#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleReloadInput.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBReloadStartedFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Definition/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/BBBRifleInputContext.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/RuntimeData/BBBRifleRuntimeData.h"

bool FBBBRifleReloadInput::IsValid() const
{
    return Sequence != INDEX_NONE;
}

bool FBBBRifleReloadInput::CanApply(const FBBBRifleInputContext &Context) const
{
    return Context.Definition.ReloadMontage
        && !Context.RuntimeData.bIsReloading
        && Context.RuntimeData.LoadedAmmo < Context.RuntimeData.AmmoCapacity;
}

void FBBBRifleReloadInput::Apply(FBBBRifleInputContext &Context) const
{
    Context.SubmitMontage(Context.Definition.ReloadMontage, Sequence, true);

    Context.RuntimeData.bIsReloading = true;
    Context.RuntimeData.bMagazineDetached = false;
    Context.RuntimeData.ReloadSequence = Sequence;
    Context.Character.SubmitInput(FBBBReloadStartedFactPacket{
        Context.Equipment.GetEquipmentId(),
        Sequence,
        Context.RuntimeData.LoadedAmmo});
}
