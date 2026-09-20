#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBAimStatePacket.h"

bool FBBBAimStatePacket::IsValid() const
{
    return !State.AimTargetWorld.ContainsNaN();
}

bool FBBBAimStatePacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return true;
}

void FBBBAimStatePacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.Aim.ApplyState(State);
}
