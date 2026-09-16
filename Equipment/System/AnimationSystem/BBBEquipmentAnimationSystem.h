#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/System/AnimationSystem/Definition/BBBEquipmentAnimationFacts.h"

class ABBBEquipmentInstance;

/** 在角色移动后统一计算并发布单件装备动画事实 */
class ABBB_EVAC_API FBBBEquipmentAnimationSystem final
{
public:
    /** 记录一次成功开火，更新动画序号 */
    void RecordFire();
    void Update(ABBBEquipmentInstance &Instance, float WorldTimeSeconds);
    void Reset();

private:
    FBBBEquipmentAnimationFacts Facts;
};
