#include "BBBWork/UBBBNexus/Character/Input/Packets/Command/BBBReloadPacket.h"

bool FBBBReloadPacket::IsValid() const
{
    return true;
}

bool FBBBReloadPacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    // 按显式处理顺序观察切枪结果，切枪或既有换弹都会拒绝新换弹
    return Context.Equipment.ActiveMainHandInstance != nullptr
        && !Context.SelectedEquipment;
}

void FBBBReloadPacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.bReload = true;
    Context.Commands.bActivateReload = true;
}
