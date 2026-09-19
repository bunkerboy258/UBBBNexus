#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBMontagePacket.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Definition/BBBCharacterMontageRequest.h"

bool FBBBMontagePacket::IsValid() const
{
    return Montage != nullptr && FMath::IsFinite(PlayRate) && PlayRate > 0.0f;
}

bool FBBBMontagePacket::CanExecute(const FBBBCharacterPacketContext &Context) const
{
    // 槽位守卫在提交时由蒙太奇请求集中校验 此处恒放行
    return true;
}

void FBBBMontagePacket::Execute(FBBBCharacterPacketContext &Context) const
{
    FBBBCharacterMontagePacket Request;
    Request.Montage = Montage;
    Request.PlayRate = PlayRate;
    Request.Sequence = Sequence;
    Request.bReload = bReload;

    if (Request.CanApply(Context.Animation, Context.Operation))
    {
        Request.Apply(Context.Animation);
    }
}
