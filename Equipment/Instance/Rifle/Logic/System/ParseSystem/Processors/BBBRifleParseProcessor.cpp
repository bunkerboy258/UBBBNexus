#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ParseSystem/Processors/BBBRifleParseProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Update/BBBRifleUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Definition/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

namespace
{
    template<typename TPacket>
    void Process(TBBBRifleInputSlot<TPacket> &Slot, FBBBRifleActionInputState &State)
    {
        if (!Slot.bActive)
        {
            return;
        }

        Slot.bActive = false;
        if (!ensureMsgf(Slot.Packet.IsValid() && Slot.Packet.CanApply(), TEXT("步枪输入包无效或不能应用")))
        {
            return;
        }

        Slot.Packet.Apply(State);
    }

    void ClearActionInput(FBBBRifleActionInputState &State)
    {
        State.bEquipRequested = false;
        State.bPrimaryRequested = false;
        State.bReloadRequested = false;
        State.bDetachMagazineRequested = false;
        State.bLoadMagazineRequested = false;
        State.bInterruptReloadRequested = false;
        State.bHasAuthorityFact = false;
        State.LoadedAmmo = 0;
        State.FireSequence = 0;
        State.ReloadSequence = 0;
        State.bIsReloading = false;
        State.bMagazineDetached = false;
    }
}

bool FBBBRifleParseProcessor::SubmitEquip(
    FBBBRifleRuntimeData &Data,
    const bool bEquipped,
    const bool bMirror,
    const FBBBEquipmentEquipLocalControlPacket &Packet)
{
    if (!bEquipped || bMirror)
    {
        return false;
    }

    BBBRifleInput::Submit(Data.Parse.InputState, Packet);
    return true;
}

bool FBBBRifleParseProcessor::SubmitEquip(
    FBBBRifleRuntimeData &Data,
    const bool bEquipped,
    const bool bMirror,
    const FBBBEquipmentEquipAuthorityFactPacket &Packet)
{
    if (!bEquipped || !bMirror)
    {
        return false;
    }

    BBBRifleInput::Submit(Data.Parse.InputState, Packet);
    return true;
}

bool FBBBRifleParseProcessor::SubmitAuthorityFact(
    FBBBRifleRuntimeData &Data,
    const bool bEquipped,
    const bool bMirror,
    const FBBBRifleActionStateAuthorityFactPacket &Payload)
{
    if (!bEquipped || !bMirror)
    {
        return false;
    }

    BBBRifleInput::Submit(Data.Parse.InputState, Payload);
    return true;
}

void FBBBRifleParseProcessor::Update(FBBBRifleUpdateContext &Context)
{
    auto &Input = Context.RuntimeData.Parse.InputState;
    auto &ActionInput = Context.RuntimeData.Action.ActionInputState;
    ClearActionInput(ActionInput);

    if (Input.Equip.bActive)
    {
        Input.Equip.bActive = false;
        ActionInput.bEquipRequested = true;
    }

    if (Input.AuthorityEquip.bActive)
    {
        Input.AuthorityEquip.bActive = false;
        ActionInput.bEquipRequested = true;
    }

    if (Context.Equipment.IsMirror())
    {
        Input.Primary.bActive = false;
        Input.Reload.bActive = false;
        Input.DetachMagazine.bActive = false;
        Input.LoadMagazine.bActive = false;
        Input.InterruptReload.bActive = false;
        Process(Input.AuthorityActionState, ActionInput);
        return;
    }

    Input.AuthorityActionState.bActive = false;

    if (Input.Primary.bActive)
    {
        Input.Primary.bActive = false;
        ActionInput.bPrimaryRequested = true;
    }

    if (Input.Reload.bActive)
    {
        Input.Reload.bActive = false;
        ActionInput.bReloadRequested = true;
    }

    Process(Input.DetachMagazine, ActionInput);
    Process(Input.LoadMagazine, ActionInput);
    Process(Input.InterruptReload, ActionInput);
}

void FBBBRifleParseProcessor::Clear(FBBBRifleRuntimeData &Data)
{
    auto &Input = Data.Parse.InputState;
    Input.Equip.bActive = false;
    Input.AuthorityEquip.bActive = false;
    Input.Primary.bActive = false;
    Input.Reload.bActive = false;
    Input.DetachMagazine.bActive = false;
    Input.LoadMagazine.bActive = false;
    Input.InterruptReload.bActive = false;
    Input.AuthorityActionState.bActive = false;
}
