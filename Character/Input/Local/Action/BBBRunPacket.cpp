#include "BBBWork/UBBBNexus/Character/Input/Local/Action/BBBRunPacket.h"

#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

bool FBBBRunPacket::IsValid() const
{
    return true;
}

bool FBBBRunPacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return true;
}

void FBBBRunPacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.Control.bRun = bRun;
}
