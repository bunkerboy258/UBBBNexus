#pragma once

class ABBBPistolEquipment;

/** 手枪Animation系统的固定调度入口 */
class FBBBPistolAnimationSystem final
{
public:
    /** @param Context	本次装备更新上下文 @return 无 */
    /** @param Equipment 卸下实例 @return 无 */
    static void Stop(ABBBPistolEquipment &Equipment);
    static void Update(ABBBPistolEquipment &Equipment);
};
