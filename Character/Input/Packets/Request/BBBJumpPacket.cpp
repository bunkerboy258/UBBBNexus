#include "BBBWork/UBBBNexus/Character/Input/Packets/Request/BBBJumpPacket.h"

bool FBBBJumpPacket::IsValid() const
{
    return true;
}

bool FBBBJumpPacket::CanApply(const FBBBCharacterPacketContext &Context) const
{
    // 跳跃允许本机预测，普通模拟代理不生成跳跃控制
    return Context.bAuthority || Context.bLocallyControlled;
}

void FBBBJumpPacket::Apply(FBBBCharacterPacketContext &Context) const
{
    Context.Operation.CommitJump();
}
