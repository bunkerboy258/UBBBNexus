#include "BBBWork/UBBBNexus/Character/Input/Packets/Base/BBBCharacterMovementPacket.h"

#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"

bool FBBBCharacterMovementPacket::IsValid() const
{
    return !MoveWorld.ContainsNaN() && !FacingWorld.ContainsNaN();
}

bool FBBBCharacterMovementPacket::CanExecute(const FBBBCharacterPacketContext &Context) const
{
    // 还原模式的移动意图由还原包直写对应域
    return !Context.Operation.IsRestoreMode();
}

void FBBBCharacterMovementPacket::Execute(FBBBCharacterPacketContext &Context) const
{
    Context.Operation.ApplyMovementSnapshot(MoveWorld, FacingWorld, bWalk, bSprint, bCrouch);
}
