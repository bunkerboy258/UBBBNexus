#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBFireFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBEquipmentActionFact.h"

bool FBBBFireFactPacket::IsValid() const
{
    return Sequence > 0;
}

bool FBBBFireFactPacket::CanExecute(const FBBBCharacterPacketContext &Context) const
{
    return true;
}

void FBBBFireFactPacket::Execute(FBBBCharacterPacketContext &Context) const
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
