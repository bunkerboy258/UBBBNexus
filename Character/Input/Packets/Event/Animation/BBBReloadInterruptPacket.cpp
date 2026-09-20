#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Animation/BBBReloadInterruptPacket.h"

bool FBBBReloadInterruptPacket::IsValid() const
{
    return true;
}

bool FBBBReloadInterruptPacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    // 还原模式不由本地动画通知驱动换弹阶段
    return true;
}

void FBBBReloadInterruptPacket::Apply(FBBBCharacterInputContext &Context) const
{
    // 序号守卫由解析状态机集中裁决
    if (Context.Operation.ReportReloadInterrupted())
    {
        Context.Commands.SubmitCancelReload(Context.Operation.GetReloadSequence());
    }
}
