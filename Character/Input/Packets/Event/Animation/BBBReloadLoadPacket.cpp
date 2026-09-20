#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Animation/BBBReloadLoadPacket.h"

bool FBBBReloadLoadPacket::IsValid() const
{
    return true;
}

bool FBBBReloadLoadPacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    // 还原模式不由本地动画通知驱动换弹阶段
    return true;
}

void FBBBReloadLoadPacket::Apply(FBBBCharacterInputContext &Context) const
{
    // 序号守卫与先卸后装守卫由解析状态机集中裁决
    if (Context.Operation.ReportMagazineLoaded())
    {
        Context.Commands.SubmitLoadMagazine(Context.Operation.GetReloadSequence());
    }
}
