#include "BBBWork/UBBBNexus/Character/Input/Mirror/State/BBBLocomotionStatePacket.h"

bool FBBBLocomotionStatePacket::IsValid() const
{
    return true;
}

bool FBBBLocomotionStatePacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return true;
}

void FBBBLocomotionStatePacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.Locomotion.Gait = Gait;
}
