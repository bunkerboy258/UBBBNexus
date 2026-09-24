#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

struct FBBBAimStatePacket final
{
    /** 是否保持瞄准 */
    bool bIsAiming = false;

    /** 角色瞄准动画权重 */
    float AimAlpha = 0.0f;

    /** 世界空间瞄准目标 */
    FVector AimTargetWorld = FVector::ZeroVector;

    bool IsValid() const;

    bool CanApply(const FBBBCharacterInputContext &Context) const;

    void Apply(FBBBCharacterInputContext &Context) const;
};
