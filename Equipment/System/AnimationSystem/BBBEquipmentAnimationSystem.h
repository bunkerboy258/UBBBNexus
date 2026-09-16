#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/System/AnimationSystem/Definition/BBBEquipmentAnimationFacts.h"

class ABBBEquipmentInstance;
struct FBBBEquipmentActionResult;

/** 在角色移动后统一计算并发布单件装备动画事实 */
class ABBB_EVAC_API FBBBEquipmentAnimationSystem final
{
public:
    void RecordAction(EBBBEquipmentActionType Type, int32 Sequence, const FBBBEquipmentActionResult &Result);
    void Update(ABBBEquipmentInstance &Instance, float WorldTimeSeconds);
    void Reset();

private:
    FBBBEquipmentAnimationFacts Facts;
};
