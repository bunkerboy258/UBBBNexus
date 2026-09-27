#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

struct FBBBAimStateAuthorityFactPacket final
{
    /** 是否保持瞄准 */
    bool bIsAiming = false;

    /** 世界空间瞄准目标 */
    FVector AimTargetWorld = FVector::ZeroVector;

    bool IsValid() const
    {
        return !AimTargetWorld.ContainsNaN();
    }

    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return true;
    }

    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.Aim.bIsAiming = bIsAiming;
        Context.Aim.AimTargetWorld = AimTargetWorld;
    }
};
