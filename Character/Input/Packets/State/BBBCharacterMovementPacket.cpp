#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBCharacterMovementPacket.h"

#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

bool FBBBCharacterMovementPacket::IsValid() const
{
    return !MoveWorld.ContainsNaN() && !FacingWorld.ContainsNaN();
}

bool FBBBCharacterMovementPacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return true;
}

void FBBBCharacterMovementPacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.Control.MoveWorld = MoveWorld.GetClampedToMaxSize(1.0f);
    Context.Control.FacingWorld = FacingWorld;
    Context.Control.bWalk = bWalk;
    Context.Control.bSprint = bSprint;
    Context.Control.bCrouch = bCrouch;
}
