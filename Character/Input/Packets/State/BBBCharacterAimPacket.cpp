#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBCharacterAimPacket.h"

#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

bool FBBBCharacterAimPacket::IsValid() const
{
    return !AimTargetWorld.ContainsNaN();
}

bool FBBBCharacterAimPacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return true;
}

void FBBBCharacterAimPacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.Control.AimTargetWorld = AimTargetWorld;
    Context.Control.bAim = bAim;
    Context.Commands.bSecondaryActive = bAim;
    Context.Commands.bSecondarySubmitted = true;
}
