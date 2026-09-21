#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Definition/BBBEquipmentAnimationFacts.h"

/** 本次步枪动画事实发布前使用的栈上暂存数据 */
struct FBBBRiflePresentationContext final
{
    /** 本次准备发布给装备动画实例的完整事实 */
    FBBBEquipmentAnimationFacts Facts;
};
