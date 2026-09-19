#include "BBBWork/UBBBNexus/Character/Input/Packets/Request/BBBFirePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Request/BBBEquipSlotPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Request/BBBReloadPacket.h"

bool FBBBFirePacket::IsValid() const
{
    return true;
}

bool FBBBFirePacket::CanExecute(const FBBBCharacterPacketContext &Context) const
{
    // 还原模式不接受本地请求 同帧切枪或换弹已批准时让步
    return !Context.Operation.IsRestoreMode()
        && !Context.Approved.HasAny<FBBBEquipSlotPacket, FBBBReloadPacket>();
}

void FBBBFirePacket::Execute(FBBBCharacterPacketContext &Context) const
{
    Context.Operation.CommitFire();
    Context.Commands.SubmitFire();
}
