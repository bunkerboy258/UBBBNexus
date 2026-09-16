#pragma once

#include "CoreMinimal.h"

class ABBBEquipmentInstance;

/** 装备实例自主驱动的唯一更新管线 */
class ABBB_EVAC_API FBBBEquipmentUpdatePipeline final
{
private:
    friend class ABBBEquipmentInstance;

    static void Update(ABBBEquipmentInstance &Instance, float WorldTimeSeconds);
};
