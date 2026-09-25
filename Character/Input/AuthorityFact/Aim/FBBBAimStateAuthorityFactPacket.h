#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

struct FBBBAimStateAuthorityFactPacket final
{
    /** 是否保持瞄准 */
    bool bIsAiming = false;

    /** 角色瞄准动画权重 */
    float AimAlpha = 0.0f;

    /** 世界空间瞄准目标 */
    FVector AimTargetWorld = FVector::ZeroVector;

    bool IsValid() const
    {
        return !AimTargetWorld.ContainsNaN()
            && FMath::IsFinite(AimAlpha)
            && AimAlpha >= 0.0f
            && AimAlpha <= 1.0f;
    }

    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return true;
    }

    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.Aim.bIsAiming = bIsAiming;
        Context.Aim.AimAlpha = AimAlpha;
        Context.Aim.AimTargetWorld = AimTargetWorld;
    }
};
