#include "BBBWork/UBBBNexus/Character/Input/Packets/Restore/BBBRestoreLocomotionPacket.h"

bool FBBBRestoreLocomotionPacket::IsValid() const
{
    return true;
}

bool FBBBRestoreLocomotionPacket::CanExecute(const FBBBCharacterPacketContext &Context) const
{
    // 还原包只在还原模式生效
    return Context.Operation.IsRestoreMode();
}

void FBBBRestoreLocomotionPacket::Execute(FBBBCharacterPacketContext &Context) const
{
    Context.Locomotion.CommitGait(Gait);
}
