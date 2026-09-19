#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBCameraPacket.h"

bool FBBBCameraPacket::IsValid() const
{
    return RecoverySpeed > 0.0f && !Impulse.ContainsNaN() && FMath::IsFinite(RecoverySpeed);
}

bool FBBBCameraPacket::CanExecute(const FBBBCharacterPacketContext &Context) const
{
    return true;
}

void FBBBCameraPacket::Execute(FBBBCharacterPacketContext &Context) const
{
    Context.Camera.Add(FBBBPlayerCameraInput{Impulse, RecoverySpeed});
}
