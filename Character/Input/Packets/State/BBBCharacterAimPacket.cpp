#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBCharacterAimPacket.h"

#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/Context/BBBCharacterPacketContext.h"

bool FBBBCharacterAimPacket::IsValid() const
{
    return !AimTargetWorld.ContainsNaN();
}

bool FBBBCharacterAimPacket::CanApply(const FBBBCharacterPacketContext &Context) const
{
    // 还原模式的瞄准状态由还原包直写对应域
    return Context.bAuthority || Context.bLocallyControlled;
}

void FBBBCharacterAimPacket::Apply(FBBBCharacterPacketContext &Context) const
{
    Context.Operation.ApplyAimSnapshot(AimTargetWorld, bAim);
}
