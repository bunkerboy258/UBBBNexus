#include "BBBWork/UBBBNexus/Character/Input/Packets/Command/BBBFirePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterOperation.h"

bool FBBBFirePacket::IsValid() const
{
    return true;
}

bool FBBBFirePacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    // 按显式处理顺序观察前序结果，切枪或换弹已承诺时让步
    return !BBBCharacterOperation::IsEquipmentSwitchPending(Context.Operation)
        && !Context.Operation.bReload;
}

void FBBBFirePacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.Operation.bFire = true;
    Context.Commands.bActivateFire = true;
}
