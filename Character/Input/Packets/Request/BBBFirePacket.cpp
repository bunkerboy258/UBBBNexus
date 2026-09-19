#include "BBBWork/UBBBNexus/Character/Input/Packets/Request/BBBFirePacket.h"

bool FBBBFirePacket::IsValid() const
{
    return true;
}

bool FBBBFirePacket::CanApply(const FBBBCharacterPacketContext &Context) const
{
    // 权威端按显式处理顺序观察前序结果，切枪或换弹已承诺时让步
    return Context.bAuthority
        && !Context.Operation.IsEquipmentSwitchPending()
        && !Context.Operation.HasReloadCommitted();
}

void FBBBFirePacket::Apply(FBBBCharacterPacketContext &Context) const
{
    Context.Operation.CommitFire();
    Context.Commands.SubmitFire();
}
