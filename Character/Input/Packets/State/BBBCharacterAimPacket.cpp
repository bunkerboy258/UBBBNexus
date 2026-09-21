#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBCharacterAimPacket.h"

#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/Context/BBBCharacterInputContext.h"

bool FBBBCharacterAimPacket::IsValid() const
{
    return !AimTargetWorld.ContainsNaN();
}

bool FBBBCharacterAimPacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    // 还原模式的瞄准状态由还原包直写对应域
    return true;
}

void FBBBCharacterAimPacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.Operation.Control.AimTargetWorld = AimTargetWorld;
    Context.Operation.Control.bAim = bAim;
    Context.Commands.SubmitSecondary(bAim);
}
