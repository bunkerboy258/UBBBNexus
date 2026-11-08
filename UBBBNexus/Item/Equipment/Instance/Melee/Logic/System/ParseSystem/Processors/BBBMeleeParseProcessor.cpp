#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/ParseSystem/Processors/BBBMeleeParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/ParseSystem/DomainData/Context/BBBMeleeParseUpdateContext.h"

namespace
{
    template<typename TPacket, typename TState>
    void Process(TBBBMeleeInputSlot<TPacket> &Slot, TState &State, const FBBBMeleeActionState &Permission)
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

void FBBBMeleeParseProcessor::Update(FBBBMeleeParseUpdateContext &Context)
{
    auto &Queue = Context.RuntimeData.Parse.InputState;
    auto &Input = Context.RuntimeData.Action.InputState;
    auto &Action = Context.RuntimeData.Action.ActionState;
    Input.bPrimary = false;
    Input.bEquip = false;
    Input.BeginActions.Reset();
    Input.EndActions.Reset();
    Input.BeginContacts.Reset();
    Input.EndContacts.Reset();

    if (Context.bCausal)
    {
        Process(Queue.LoadAmmoLocalControl, Input, Action);
        Process(Queue.InterruptReloadLocalControl, Input, Action);
        Process(Queue.BlockFireLocalControl, Input, Action);
        Process(Queue.AllowFireLocalControl, Input, Action);
        Process(Queue.SecondaryLocalControl, Input, Action);
        Process(Queue.ShowLocalControl, Action, Action);

        Process(Queue.HideLocalControl, Action, Action);

        Process(Queue.Equip, Input, Action);
        Process(Queue.Attack, Input, Action);
        Process(Queue.BeginAction, Input, Action);
        Process(Queue.BeginContact, Input, Action);
        Process(Queue.EndContact, Input, Action);
        Process(Queue.EndAction, Input, Action);
        Clear(Context.RuntimeData);
        return;
    }

    Process(Queue.VisibilityRemoteMessage, Action, Action);
    Process(Queue.VisibilityAuthorityFact, Action, Action);
    Process(Queue.AuthorityEquip, Input, Action);
    Process(Queue.RemoteAttackStart, Action, Action);
    Process(Queue.RemoteAttackEnd, Action, Action);
    Process(Queue.AuthorityAttackStart, Action, Action);
    Process(Queue.AuthorityAttackEnd, Action, Action);

    Clear(Context.RuntimeData);
}

void FBBBMeleeParseProcessor::Clear(FBBBMeleeRuntimeData &Data)
{
    Data.Parse.InputState.LoadAmmoLocalControl.bActive = false;
    Data.Parse.InputState.LoadAmmoLocalControl.Packet = {};
    Data.Parse.InputState.InterruptReloadLocalControl.bActive = false;
    Data.Parse.InputState.InterruptReloadLocalControl.Packet = {};
    Data.Parse.InputState.BlockFireLocalControl.bActive = false;
    Data.Parse.InputState.BlockFireLocalControl.Packet = {};
    Data.Parse.InputState.AllowFireLocalControl.bActive = false;
    Data.Parse.InputState.AllowFireLocalControl.Packet = {};
    Data.Parse.InputState.SecondaryLocalControl.bActive = false;
    Data.Parse.InputState.SecondaryLocalControl.Packet = {};
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
    Data.Parse.InputState.Attack.bActive = false;
    Data.Parse.InputState.Attack.Packet = {};
    Data.Parse.InputState.BeginAction.bActive = false;
    Data.Parse.InputState.BeginAction.Packet = {};
    Data.Parse.InputState.BeginContact.bActive = false;
    Data.Parse.InputState.BeginContact.Packet = {};
    Data.Parse.InputState.EndContact.bActive = false;
    Data.Parse.InputState.EndContact.Packet = {};
    Data.Parse.InputState.EndAction.bActive = false;
    Data.Parse.InputState.EndAction.Packet = {};
    Data.Parse.InputState.RemoteAttackStart.bActive = false;
    Data.Parse.InputState.RemoteAttackStart.Packet = {};
    Data.Parse.InputState.RemoteAttackEnd.bActive = false;
    Data.Parse.InputState.RemoteAttackEnd.Packet = {};
    Data.Parse.InputState.AuthorityAttackStart.bActive = false;
    Data.Parse.InputState.AuthorityAttackStart.Packet = {};
    Data.Parse.InputState.AuthorityAttackEnd.bActive = false;
    Data.Parse.InputState.AuthorityAttackEnd.Packet = {};
}
