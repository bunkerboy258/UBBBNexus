#include "BBBWork/UBBBNexus/Character/Input/Packets/Restore/BBBRestoreAimPacket.h"

bool FBBBRestoreAimPacket::IsValid() const
{
    return !State.AimTargetWorld.ContainsNaN();
}

bool FBBBRestoreAimPacket::CanExecute(const FBBBCharacterPacketContext &Context) const
{
    // 还原包只在还原模式生效
    return Context.Operation.IsRestoreMode();
}

void FBBBRestoreAimPacket::Execute(FBBBCharacterPacketContext &Context) const
{
    Context.Aim.ApplyRestoredState(State);
}
