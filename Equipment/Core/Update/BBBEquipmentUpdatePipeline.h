#pragma once

#include "CoreMinimal.h"

class ABBBEquipmentInstance;

/** 由角色更新组调度的装备阶段管线 */
class ABBB_EVAC_API FBBBEquipmentUpdatePipeline final
{
public:
    static void Advance(ABBBEquipmentInstance &Instance, float WorldTimeSeconds);
    static void LateUpdate(ABBBEquipmentInstance &Instance, float WorldTimeSeconds);
};
