#include "BBBWork/UBBBNexus/Character/Input/Local/Action/BBBCrouchPacket.h"

#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

bool FBBBCrouchPacket::IsValid() const
{
    return true;
}

bool FBBBCrouchPacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return true;
}

void FBBBCrouchPacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.Control.bCrouch = bCrouch;
}
