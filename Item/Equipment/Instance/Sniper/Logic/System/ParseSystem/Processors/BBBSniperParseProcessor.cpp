#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/ParseSystem/Processors/BBBSniperParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/ParseSystem/DomainData/Context/BBBSniperParseUpdateContext.h"

namespace
{
    template<typename TPacket, typename TState>
    void Process(TBBBSniperInputSlot<TPacket> &Slot, TState &State, const FBBBSniperActionState &Permission)
    {
        if (!Slot.bActive) return;
        Slot.bActive = false;
        if (Slot.Packet.IsValid() && Slot.Packet.CanApply(Permission))
        {
            Slot.Packet.Apply(State);
        }
        Slot.Packet = {};
    }
}

void FBBBSniperParseProcessor::Update(FBBBSniperParseUpdateContext &Context)
{
    auto &Queue = Context.RuntimeData.Parse.InputState;
    auto &Input = Context.RuntimeData.Action.ActionInputState;
    auto &Action = Context.RuntimeData.Action.ActionState;
    Input.bEquipRequested = false;
    Input.bBlockFireRequested = false;
    Input.bAllowFireRequested = false;
    Input.bPrimaryRequested = false;
    Input.bReloadRequested = false;
    Input.bLoadMagazineRequested = false;
    Input.bInterruptReloadRequested = false;

    if (Context.bCausal)
    {
        Process(Queue.EndContactLocalControl, Input, Action);
        Process(Queue.EndActionLocalControl, Input, Action);
        Process(Queue.BeginContactLocalControl, Input, Action);
        Process(Queue.BeginActionLocalControl, Input, Action);
        Process(Queue.ShowLocalControl, Action, Action);

        Process(Queue.HideLocalControl, Action, Action);

        Process(Queue.Equip, Input, Action);
        Process(Queue.Fire, Input, Action);
        Process(Queue.Reload, Input, Action);
        Process(Queue.BlockFire, Input, Action);
        Process(Queue.AllowFire, Input, Action);
        Process(Queue.LoadMagazine, Input, Action);
        Process(Queue.InterruptReload, Input, Action);
        Clear(Context.RuntimeData);
        return;
    }

    Process(Queue.VisibilityRemoteMessage, Action, Action);
    Process(Queue.VisibilityAuthorityFact, Action, Action);
    Process(Queue.AuthorityEquip, Input, Action);
    Process(Queue.RemoteFire, Action, Action);
    Process(Queue.RemoteReloadStart, Action, Action);
    Process(Queue.RemoteReloadEnd, Action, Action);
    Process(Queue.AuthorityFire, Action, Action);
    Process(Queue.AuthorityReloadStart, Action, Action);
    Process(Queue.AuthorityReloadEnd, Action, Action);

    Clear(Context.RuntimeData);
}

void FBBBSniperParseProcessor::Clear(FBBBSniperRuntimeData &Data)
{
    Data.Parse.InputState.EndContactLocalControl.bActive = false;
    Data.Parse.InputState.EndContactLocalControl.Packet = {};
    Data.Parse.InputState.EndActionLocalControl.bActive = false;
    Data.Parse.InputState.EndActionLocalControl.Packet = {};
    Data.Parse.InputState.BeginContactLocalControl.bActive = false;
    Data.Parse.InputState.BeginContactLocalControl.Packet = {};
    Data.Parse.InputState.BeginActionLocalControl.bActive = false;
    Data.Parse.InputState.BeginActionLocalControl.Packet = {};
    Data.Parse.InputState.VisibilityAuthorityFact.bActive = false;
    Data.Parse.InputState.VisibilityAuthorityFact.Packet = {};

    Data.Parse.InputState.VisibilityRemoteMessage.bActive = false;
    Data.Parse.InputState.VisibilityRemoteMessage.Packet = {};

    Data.Parse.InputState.ShowLocalControl.bActive = false;
    Data.Parse.InputState.ShowLocalControl.Packet = {};

    Data.Parse.InputState.HideLocalControl.bActive = false;
    Data.Parse.InputState.HideLocalControl.Packet = {};

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
