#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBFireFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipmentActionFact.h"

bool FBBBFireFactPacket::IsValid() const
{
    return Sequence > 0;
}

bool FBBBFireFactPacket::CanApply(const FBBBCharacterPacketContext &Context) const
{
    return true;
}

void FBBBFireFactPacket::Apply(FBBBCharacterPacketContext &Context) const
{
    if (Context.Operation.IsRestoreMode())
    {
        // 远端开火意味着其换弹不再有效 本地追踪同步中止
        if (Context.Operation.IsReloadInProgress())
        {
            Context.Operation.CancelReload();
        }

        Context.Commands.SubmitRestoredAction(ToFact());
    }

    Context.Events.AddAction(ToFact());
}

FBBBEquipmentActionFact FBBBFireFactPacket::ToFact() const
{
    return FBBBEquipmentActionFact{PacketId, EquipmentId, Sequence, LoadedAmmo};
}
