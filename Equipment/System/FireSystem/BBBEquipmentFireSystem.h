#pragma once

#include "CoreMinimal.h"

class ABBBEquipmentInstance;
struct FBBBEquipmentActionResult;

/** 单投射物开火与表现系统 */
class ABBB_EVAC_API FBBBEquipmentFireSystem final
{
public:
    bool Fire(ABBBEquipmentInstance &Instance, FBBBEquipmentActionResult &OutResult) const;
};
