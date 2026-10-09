#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/ParseSystem/Processors/BBBSMGParseProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/ActionSystem/DomainData/Context/BBBSMGUpdateContext.h"

namespace
{
    template<typename TPacket, typename TState>
    void Process(TBBBSMGInputSlot<TPacket> &Slot, TState &State)
    {
        if (!Slot.bActive)
        {
            return;
        }

        Slot.bActive = false;
        if (Slot.Packet.IsValid())
        {
            if constexpr (std::is_same_v<TState, FBBBSMGActionState>)
            {
                if (Slot.Packet.CanApply(State))
                {
                    Slot.Packet.Apply(State);
                }
            }

            if constexpr (!std::is_same_v<TState, FBBBSMGActionState>)
            {
                if (Slot.Packet.CanApply())
                {
                    Slot.Packet.Apply(State);
                }
            }
        }

        Slot.Packet = {};
    }
}

void FBBBSMGParseProcessor::Update(FBBBSMGUpdateContext &Context)
{
    auto &Queue = Context.RuntimeData.Parse.InputState;
    auto &Input = Context.RuntimeData.Action.ActionInputState;
    auto &Action = Context.RuntimeData.Action.ActionState;
    Input.bEquipRequested = false;
    Input.bActionPermissionReceived = false;
    Input.bBlockFireRequested = false;
    Input.bAllowFireRequested = false;
    Input.bPrimaryRequested = false;
    Input.bReloadRequested = false;
    Input.bLoadMagazineRequested = false;
    Input.bInterruptReloadRequested = false;

    if (Context.bCausal)
    {
        Process(Queue.Equip, Input);
        Process(Queue.Fire, Input);
        Process(Queue.Reload, Input);
        Process(Queue.BlockFire, Input);
        Process(Queue.AllowFire, Input);
        Process(Queue.LoadMagazine, Input);
        Process(Queue.InterruptReload, Input);
        Process(Queue.ActionPermission, Input);
        Clear(Context.RuntimeData);
        return;
    }

    Process(Queue.AuthorityEquip, Input);
    Process(Queue.RemoteFire, Action);
    Process(Queue.RemoteReloadStart, Action);
    Process(Queue.RemoteReloadEnd, Action);
    Process(Queue.AuthorityFire, Action);
    Process(Queue.AuthorityReloadStart, Action);
    Process(Queue.AuthorityReloadEnd, Action);

    Clear(Context.RuntimeData);
}

void FBBBSMGParseProcessor::Clear(FBBBSMGRuntimeData &Data)
{
    Data.Parse.InputState.Equip.bActive = false;
    Data.Parse.InputState.Equip.Packet = {};
    Data.Parse.InputState.AuthorityEquip.bActive = false;
    Data.Parse.InputState.AuthorityEquip.Packet = {};
    Data.Parse.InputState.Fire.bActive = false;
    Data.Parse.InputState.Fire.Packet = {};
    Data.Parse.InputState.Reload.bActive = false;
    Data.Parse.InputState.Reload.Packet = {};
    Data.Parse.InputState.BlockFire.bActive = false;
    Data.Parse.InputState.BlockFire.Packet = {};
    Data.Parse.InputState.AllowFire.bActive = false;
    Data.Parse.InputState.AllowFire.Packet = {};
    Data.Parse.InputState.LoadMagazine.bActive = false;
    Data.Parse.InputState.LoadMagazine.Packet = {};
    Data.Parse.InputState.InterruptReload.bActive = false;
    Data.Parse.InputState.InterruptReload.Packet = {};
    Data.Parse.InputState.ActionPermission.bActive = false;
    Data.Parse.InputState.ActionPermission.Packet = {};
    Data.Parse.InputState.RemoteFire.bActive = false;
    Data.Parse.InputState.RemoteFire.Packet = {};
    Data.Parse.InputState.RemoteReloadStart.bActive = false;
    Data.Parse.InputState.RemoteReloadStart.Packet = {};
    Data.Parse.InputState.RemoteReloadEnd.bActive = false;
    Data.Parse.InputState.RemoteReloadEnd.Packet = {};
    Data.Parse.InputState.AuthorityFire.bActive = false;
    Data.Parse.InputState.AuthorityFire.Packet = {};
    Data.Parse.InputState.AuthorityReloadStart.bActive = false;
    Data.Parse.InputState.AuthorityReloadStart.Packet = {};
    Data.Parse.InputState.AuthorityReloadEnd.bActive = false;
    Data.Parse.InputState.AuthorityReloadEnd.Packet = {};
}
