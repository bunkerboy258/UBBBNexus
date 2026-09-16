#pragma once

#include "CoreMinimal.h"

class ABBBEquipmentInstance;

/** 单投射物开火与表现系统 */
class ABBB_EVAC_API FBBBEquipmentFireSystem final
{
public:
    bool Fire(ABBBEquipmentInstance &Instance, int32 Sequence) const;
};
