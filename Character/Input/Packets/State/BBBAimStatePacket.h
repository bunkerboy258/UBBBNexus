#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

struct FBBBAimStatePacket final
{
    /** 是否保持瞄准 */
    bool bIsAiming = false;

    /** 世界空间瞄准目标 */
    FVector AimTargetWorld = FVector::ZeroVector;

    bool IsValid() const;

    bool CanApply(const FBBBCharacterInputContext &Context) const;

    void Apply(FBBBCharacterInputContext &Context) const;
};
