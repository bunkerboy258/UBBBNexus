#include "BBBWork/UBBBNexus/Character/Input/Packets/Base/BBBCharacterAimPacket.h"

#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"

bool FBBBCharacterAimPacket::IsValid() const
{
    return !AimTargetWorld.ContainsNaN();
}

bool FBBBCharacterAimPacket::CanExecute(const FBBBCharacterPacketContext &Context) const
{
    // 还原模式的瞄准状态由还原包直写对应域
    return !Context.Operation.IsRestoreMode();
}

void FBBBCharacterAimPacket::Execute(FBBBCharacterPacketContext &Context) const
{
    Context.Operation.ApplyAimSnapshot(AimTargetWorld, bAim);
}
