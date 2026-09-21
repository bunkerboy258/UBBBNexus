#include "BBBWork/UBBBNexus/Character/Input/Packets/Command/BBBJumpPacket.h"

bool FBBBJumpPacket::IsValid() const
{
    return true;
}

bool FBBBJumpPacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    // 跳跃允许本机预测，普通模拟代理不生成跳跃控制
    return true;
}

void FBBBJumpPacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.Operation.Control.bJump = true;
}
