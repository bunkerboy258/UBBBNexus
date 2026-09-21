#include "BBBWork/UBBBNexus/Character/Input/Packets/Command/BBBFirePacket.h"

bool FBBBFirePacket::IsValid() const
{
    return true;
}

bool FBBBFirePacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    // 权威端按显式处理顺序观察前序结果，切枪或换弹已承诺时让步
    return !Context.Operation.IsEquipmentSwitchPending()
        && !Context.Operation.HasReloadCommitted();
}

void FBBBFirePacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.Operation.bFire = true;
    Context.Commands.SubmitFire();
}
