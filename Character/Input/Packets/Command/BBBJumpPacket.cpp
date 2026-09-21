#include "BBBWork/UBBBNexus/Character/Input/Packets/Command/BBBJumpPacket.h"

bool FBBBJumpPacket::IsValid() const
{
    return true;
}

bool FBBBJumpPacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return true;
}

void FBBBJumpPacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.Control.bJump = true;
}
