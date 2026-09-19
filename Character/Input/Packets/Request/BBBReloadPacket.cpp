#include "BBBWork/UBBBNexus/Character/Input/Packets/Request/BBBReloadPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Request/BBBEquipSlotPacket.h"

bool FBBBReloadPacket::IsValid() const
{
    return true;
}

bool FBBBReloadPacket::CanExecute(const FBBBCharacterPacketContext &Context) const
{
    // 还原模式不接受本地请求 空手 换弹进行中与同帧切枪都拒绝换弹
    return !Context.Operation.IsRestoreMode()
        && Context.Equipment.GetActiveMainHandInstance() != nullptr
        && !Context.Operation.IsReloadInProgress()
        && !Context.Approved.HasAny<FBBBEquipSlotPacket>();
}

void FBBBReloadPacket::Execute(FBBBCharacterPacketContext &Context) const
{
    Context.Operation.CommitReload();
    Context.Commands.SubmitReload();
}
