#include "BBBWork/UBBBNexus/Character/Input/Packets/Request/BBBJumpPacket.h"

bool FBBBJumpPacket::IsValid() const
{
    return true;
}

bool FBBBJumpPacket::CanExecute(const FBBBCharacterPacketContext &Context) const
{
    // 还原模式不接受本地请求
    return !Context.Operation.IsRestoreMode();
}

void FBBBJumpPacket::Execute(FBBBCharacterPacketContext &Context) const
{
    Context.Operation.CommitJump();
}
