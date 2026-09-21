#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

struct FBBBEquipmentStatePacket final
{
    FName EquipmentId = NAME_None;

    bool IsValid() const;

    bool CanApply(const FBBBCharacterInputContext &Context) const;

    void Apply(FBBBCharacterInputContext &Context) const;
};
