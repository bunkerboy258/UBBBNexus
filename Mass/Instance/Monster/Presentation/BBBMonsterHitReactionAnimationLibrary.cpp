#include "BBBMonsterHitReactionAnimationLibrary.h"

FRotator UBBBMonsterHitReactionAnimationLibrary::CalculateHitBoneRotation(const FVector Direction, const float Age,
    const int32 Region, const int32 BoneRegion, const bool bAlive)
{
    if (!bAlive || Age < 0.0f || Age > 0.65f || !FMath::IsFinite(Age) || Direction.ContainsNaN())
    {
        return FRotator::ZeroRotator;
    }

    const float Weight = Region == BoneRegion ? 1.0f : BoneRegion == static_cast<int32>(EBBBMonsterHitRegion::Torso) ? 0.3f : 0.0f;
    const float Envelope = (1.0f - FMath::Exp(-Age * 65.0f)) * FMath::Exp(-Age * 9.0f);
    const FVector Axis = FVector::CrossProduct(FVector::UpVector, Direction.GetSafeNormal()).GetSafeNormal();
    return FQuat(Axis.IsNearlyZero() ? FVector::RightVector : Axis,
        FMath::DegreesToRadians(18.0f * Weight * Envelope)).Rotator();
}
