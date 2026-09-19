#include "BBBWork/UBBBNexus/Character/Input/Packets/Replication/BBBRestoreAimPacket.h"

bool FBBBRestoreAimPacket::IsValid() const
{
    return !State.AimTargetWorld.ContainsNaN();
}

bool FBBBRestoreAimPacket::CanApply(const FBBBCharacterPacketContext &Context) const
{
    // 还原包只在还原模式生效
    return Context.Operation.IsRestoreMode();
}

void FBBBRestoreAimPacket::Apply(FBBBCharacterPacketContext &Context) const
{
    Context.Aim.ApplyRestoredState(State);
}
