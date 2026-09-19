#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBEquipFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBEquipmentActionFact.h"

bool FBBBEquipFactPacket::IsValid() const
{
    return Sequence > 0;
}

bool FBBBEquipFactPacket::CanExecute(const FBBBCharacterPacketContext &Context) const
{
    return true;
}

void FBBBEquipFactPacket::Execute(FBBBCharacterPacketContext &Context) const
{
    // 还原模式下事实要交给镜像装备还原 本地模式仅留档供网络上传
    if (Context.Operation.IsRestoreMode())
    {
        Context.Commands.SubmitRestoredAction(ToFact());
    }

    Context.Events.AddAction(ToFact());
}

FBBBEquipmentActionFact FBBBEquipFactPacket::ToFact() const
{
    return FBBBEquipmentActionFact{PacketId, EquipmentId, Sequence, LoadedAmmo};
}
