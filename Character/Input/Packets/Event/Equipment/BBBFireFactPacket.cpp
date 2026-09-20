#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBFireFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipmentActionFact.h"

bool FBBBFireFactPacket::IsValid() const
{
    return Sequence > 0;
}

bool FBBBFireFactPacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return true;
}

void FBBBFireFactPacket::Apply(FBBBCharacterInputContext &Context) const
{
    if (Context.Operation.IsReloadInProgress())
    {
        Context.Operation.CancelReload();
    }

    Context.Commands.SubmitFact(ToFact());
    Context.Events.AddAction(ToFact());
}

FBBBEquipmentActionFact FBBBFireFactPacket::ToFact() const
{
    return FBBBEquipmentActionFact{PacketId, EquipmentId, Sequence, LoadedAmmo};
}
