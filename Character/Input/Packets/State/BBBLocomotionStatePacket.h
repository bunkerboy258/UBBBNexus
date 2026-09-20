#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/Context/BBBCharacterInputContext.h"

struct FBBBLocomotionStatePacket final
{
    EBBBCharacterGait Gait = EBBBCharacterGait::Run;

    bool IsValid() const;

    bool CanApply(const FBBBCharacterInputContext &Context) const;

    void Apply(FBBBCharacterInputContext &Context) const;
};
