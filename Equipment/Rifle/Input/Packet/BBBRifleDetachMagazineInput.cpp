#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleDetachMagazineInput.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBMagazineDetachedFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/DomainData/Context/BBBRifleInputContext.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/RuntimeData/BBBRifleRuntimeData.h"

bool FBBBRifleDetachMagazineInput::IsValid() const
{
    return true;
}

bool FBBBRifleDetachMagazineInput::CanApply(const FBBBRifleInputContext &Context) const
{
    const auto &State = Context.RuntimeData.Rifle.ReadRifleActionState();
    if (!State.bIsReloading || State.bMagazineDetached)
    {
        return false;
    }

    return Sequence == INDEX_NONE || Sequence == State.ReloadSequence;
}

void FBBBRifleDetachMagazineInput::Apply(FBBBRifleInputContext &Context) const
{
    auto &State = Context.RuntimeData.Rifle.Action;
    State.LoadedAmmo = 0;
    State.bMagazineDetached = true;
    Context.Character.SubmitInput(FBBBMagazineDetachedFactPacket{
        Context.Equipment.GetEquipmentId(),
        State.ReloadSequence,
        State.LoadedAmmo});
}
