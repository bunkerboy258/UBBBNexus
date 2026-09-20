#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBCharacterMovementPacket.h"

#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/Context/BBBCharacterInputContext.h"

bool FBBBCharacterMovementPacket::IsValid() const
{
    return !MoveWorld.ContainsNaN() && !FacingWorld.ContainsNaN();
}

bool FBBBCharacterMovementPacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    // 还原模式的移动意图由还原包直写对应域
    return true;
}

void FBBBCharacterMovementPacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.Operation.ApplyMovementSnapshot(MoveWorld, FacingWorld, bWalk, bSprint, bCrouch);
}
