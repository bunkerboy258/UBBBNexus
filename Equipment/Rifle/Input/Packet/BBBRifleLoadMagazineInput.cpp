#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleLoadMagazineInput.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBMagazineLoadedFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/DomainData/Context/BBBRifleInputContext.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/RuntimeData/BBBRifleRuntimeData.h"

bool FBBBRifleLoadMagazineInput::IsValid() const
{
    return true;
}

bool FBBBRifleLoadMagazineInput::CanApply(const FBBBRifleInputContext &Context) const
{
    const auto &State = Context.RuntimeData.Rifle.ReadRifleActionState();
    if (!State.bIsReloading || !State.bMagazineDetached)
    {
        return false;
    }

    return Sequence == INDEX_NONE || Sequence == State.ReloadSequence;
}

void FBBBRifleLoadMagazineInput::Apply(FBBBRifleInputContext &Context) const
{
    auto &State = Context.RuntimeData.Rifle.Action;
    State.LoadedAmmo = State.AmmoCapacity;
    State.bIsReloading = false;
    State.bMagazineDetached = false;
    Context.Character.SubmitInput(FBBBMagazineLoadedFactPacket{
        Context.Equipment.GetEquipmentId(),
        State.ReloadSequence,
        State.LoadedAmmo});
}
