#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBAimStatePacket.h"

bool FBBBAimStatePacket::IsValid() const
{
    return !AimTargetWorld.ContainsNaN()
        && FMath::IsFinite(AimAlpha)
        && AimAlpha >= 0.0f
        && AimAlpha <= 1.0f;
}

bool FBBBAimStatePacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return true;
}

void FBBBAimStatePacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.Aim.bIsAiming = bIsAiming;
    Context.Aim.AimAlpha = AimAlpha;
    Context.Aim.AimTargetWorld = AimTargetWorld;
}
