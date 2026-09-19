#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Animation/BBBReloadDetachPacket.h"

bool FBBBReloadDetachPacket::IsValid() const
{
    return Sequence > 0;
}

bool FBBBReloadDetachPacket::CanApply(const FBBBCharacterPacketContext &Context) const
{
    // 还原模式不由本地动画通知驱动换弹阶段
    return !Context.Operation.IsRestoreMode();
}

void FBBBReloadDetachPacket::Apply(FBBBCharacterPacketContext &Context) const
{
    // 序号守卫与阶段守卫由解析状态机集中裁决
    if (Context.Operation.ReportMagazineDetached(Sequence))
    {
        Context.Commands.SubmitDetachMagazine(Sequence);
    }
}
