#pragma once

#include "CoreMinimal.h"

class ABBBEquipmentInstance;

/** 装备实例自主驱动的唯一更新管线 */
class ABBB_EVAC_API FBBBEquipmentUpdatePipeline final
{
public:
    static void Update(ABBBEquipmentInstance &Instance, float WorldTimeSeconds);
};
