#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBCameraPacket.h"

bool FBBBCameraPacket::IsValid() const
{
    return RecoverySpeed > 0.0f && !Impulse.ContainsNaN() && FMath::IsFinite(RecoverySpeed);
}

bool FBBBCameraPacket::CanApply(const FBBBCharacterPacketContext &Context) const
{
    return true;
}

void FBBBCameraPacket::Apply(FBBBCharacterPacketContext &Context) const
{
    // 相机输入由提交方预先累计，角色黑板只保留最后一次完整结果
    Context.Camera = FBBBPlayerCameraInput{Impulse, RecoverySpeed};
}
