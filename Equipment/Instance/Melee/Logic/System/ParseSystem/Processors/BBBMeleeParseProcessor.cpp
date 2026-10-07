#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ParseSystem/Processors/BBBMeleeParseProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ActionSystem/DomainData/Context/BBBMeleeUpdateContext.h"

namespace
{
    template<typename TPacket, typename TState>
    void Process(TBBBMeleeInputSlot<TPacket> &Slot, TState &State)
    {
        if (!Slot.bActive)
        {
            return;
        }

        Slot.bActive = false;
        if (Slot.Packet.IsValid())
        {
            if constexpr (std::is_same_v<TState, FBBBMeleeActionState>)
            {
                if (Slot.Packet.CanApply(State))
                {
                    Slot.Packet.Apply(State);
                }
            }

            if constexpr (!std::is_same_v<TState, FBBBMeleeActionState>)
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

void FBBBMeleeParseProcessor::Update(FBBBMeleeUpdateContext &Context)
{
    auto &Queue = Context.Data.Parse.InputState;
    auto &Input = Context.Data.Action.InputState;
    auto &Action = Context.Data.Action.ActionState;
    Input.bPrimary = false;
    Input.bEquip = false;
    Input.bUnequip = false;
    Input.bActionPermissionReceived = false;
    Input.BeginActions.Reset();
    Input.EndActions.Reset();
    Input.BeginContacts.Reset();
    Input.EndContacts.Reset();

    if (Context.bCausal)
    {
        Process(Queue.Equip, Input);
        Process(Queue.Unequip, Input);
        Process(Queue.Attack, Input);
        Process(Queue.ActionPermission, Input);
        Process(Queue.BeginAction, Input);
        Process(Queue.BeginContact, Input);
        Process(Queue.EndContact, Input);
        Process(Queue.EndAction, Input);
        Clear(Context.Data);
        return;
    }

    Process(Queue.AuthorityEquip, Input);
    Process(Queue.AuthorityUnequip, Input);
    Process(Queue.RemoteAttackStart, Action);
    Process(Queue.RemoteAttackEnd, Action);
    Process(Queue.AuthorityAttackStart, Action);
    Process(Queue.AuthorityAttackEnd, Action);

    Clear(Context.Data);
}

void FBBBMeleeParseProcessor::Clear(FBBBMeleeRuntimeData &Data)
{
    Data.Parse.InputState.Equip.bActive = false;
    Data.Parse.InputState.Equip.Packet = {};
    Data.Parse.InputState.AuthorityEquip.bActive = false;
    Data.Parse.InputState.AuthorityEquip.Packet = {};
    Data.Parse.InputState.Unequip.bActive = false;
    Data.Parse.InputState.Unequip.Packet = {};
    Data.Parse.InputState.AuthorityUnequip.bActive = false;
    Data.Parse.InputState.AuthorityUnequip.Packet = {};
    Data.Parse.InputState.Attack.bActive = false;
    Data.Parse.InputState.Attack.Packet = {};
    Data.Parse.InputState.ActionPermission.bActive = false;
    Data.Parse.InputState.ActionPermission.Packet = {};
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
