#include "BBBWork/UBBBNexus/Character/Input/Packets/Command/BBBReloadPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterOperation.h"

bool FBBBReloadPacket::IsValid() const
{
    return true;
}

bool FBBBReloadPacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    // 按显式处理顺序观察切枪结果，切枪或既有换弹都会拒绝新换弹
    return Context.Equipment.ActiveMainHandInstance != nullptr
        && !BBBCharacterOperation::IsReloadInProgress(Context.Operation)
        && !BBBCharacterOperation::IsEquipmentSwitchPending(Context.Operation);
}

void FBBBReloadPacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.Operation.bReload = true;
    Context.Commands.bActivateReload = true;
}
