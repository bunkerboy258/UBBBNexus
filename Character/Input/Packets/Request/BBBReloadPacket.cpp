#include "BBBWork/UBBBNexus/Character/Input/Packets/Request/BBBReloadPacket.h"

bool FBBBReloadPacket::IsValid() const
{
    return true;
}

bool FBBBReloadPacket::CanApply(const FBBBCharacterPacketContext &Context) const
{
    // 权威端按显式处理顺序观察切枪结果，切枪或既有换弹都会拒绝新换弹
    return Context.bAuthority
        && Context.Equipment.GetActiveMainHandInstance() != nullptr
        && !Context.Operation.IsReloadInProgress()
        && !Context.Operation.IsEquipmentSwitchPending();
}

void FBBBReloadPacket::Apply(FBBBCharacterPacketContext &Context) const
{
    Context.Operation.CommitReload();
    Context.Commands.SubmitReload();
}
