#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Animation/BBBReloadInterruptPacket.h"

bool FBBBReloadInterruptPacket::IsValid() const
{
    return Sequence > 0;
}

bool FBBBReloadInterruptPacket::CanApply(const FBBBCharacterPacketContext &Context) const
{
    // 还原模式不由本地动画通知驱动换弹阶段
    return !Context.Operation.IsRestoreMode();
}

void FBBBReloadInterruptPacket::Apply(FBBBCharacterPacketContext &Context) const
{
    // 序号守卫由解析状态机集中裁决
    if (Context.Operation.ReportReloadInterrupted(Sequence))
    {
        Context.Commands.SubmitCancelReload(Sequence);
    }
}
