#pragma once

class ABBBLMGEquipment;

/** 轻机枪Animation系统的固定调度入口 */
class FBBBLMGAnimationSystem final
{
public:
    /** @param Context	本次装备更新上下文 @return 无 */
    /** @param Equipment 卸下实例 @return 无 */
    static void Stop(ABBBLMGEquipment &Equipment);
    static void Update(ABBBLMGEquipment &Equipment);
};
