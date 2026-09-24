#include "BBBWork/UBBBNexus/Character/Input/Mirror/State/BBBRunStatePacket.h"

#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

bool FBBBRunStatePacket::IsValid() const
{
    return true;
}

bool FBBBRunStatePacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return true;
}

void FBBBRunStatePacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.Locomotion.bRun = bRun;
}
