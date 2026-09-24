#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ParseSystem/Processors/BBBRifleParseProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/BBBRifleUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Definition/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

namespace
{
    template<typename TPacket>
    void Process(TBBBEquipmentInputSlot<TPacket> &Slot, FBBBRifleActionInputState &State)
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
        State.bFireRequested = false;
        State.bReloadRequested = false;
        State.bDetachMagazineRequested = false;
        State.bLoadMagazineRequested = false;
        State.bInterruptReloadRequested = false;
        State.bHasNetworkState = false;
        State.LoadedAmmo = 0;
        State.FireSequence = 0;
        State.ReloadSequence = 0;
        State.bIsReloading = false;
        State.bMagazineDetached = false;
    }
}

void FBBBRifleParseProcessor::Update(FBBBRifleUpdateContext &Context)
{
    auto &Input = Context.RuntimeData.Parse.InputState;
    auto &ActionInput = Context.RuntimeData.Action.ActionInputState;
    ClearActionInput(ActionInput);
    Process(Input.Equip, ActionInput);

    if (Context.Equipment.IsMirror())
    {
        Input.Fire.bActive = false;
        Input.Reload.bActive = false;
        Input.DetachMagazine.bActive = false;
        Input.LoadMagazine.bActive = false;
        Input.InterruptReload.bActive = false;
        Process(Input.NetworkState, ActionInput);
        return;
    }

    Input.NetworkState.bActive = false;
    Process(Input.Fire, ActionInput);
    Process(Input.Reload, ActionInput);
    Process(Input.DetachMagazine, ActionInput);
    Process(Input.LoadMagazine, ActionInput);
    Process(Input.InterruptReload, ActionInput);
}

void FBBBRifleParseProcessor::Clear(FBBBRifleRuntimeData &Data)
{
    auto &Input = Data.Parse.InputState;
    Input.Equip.bActive = false;
    Input.Fire.bActive = false;
    Input.Reload.bActive = false;
    Input.DetachMagazine.bActive = false;
    Input.LoadMagazine.bActive = false;
    Input.InterruptReload.bActive = false;
    Input.NetworkState.bActive = false;
}
