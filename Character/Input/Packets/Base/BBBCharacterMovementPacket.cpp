#include "BBBWork/UBBBNexus/Character/Input/Packets/Base/BBBCharacterMovementPacket.h"

#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"

bool FBBBCharacterMovementPacket::IsValid() const
{
    return !MoveWorld.ContainsNaN() && !FacingWorld.ContainsNaN();
}

bool FBBBCharacterMovementPacket::CanApply(const FBBBCharacterPacketContext &Context) const
{
    // 还原模式的移动意图由还原包直写对应域
    return Context.bAuthority || Context.bLocallyControlled;
}

void FBBBCharacterMovementPacket::Apply(FBBBCharacterPacketContext &Context) const
{
    Context.Operation.ApplyMovementSnapshot(MoveWorld, FacingWorld, bWalk, bSprint, bCrouch);
}
