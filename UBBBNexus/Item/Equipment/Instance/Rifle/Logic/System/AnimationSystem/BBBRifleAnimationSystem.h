#pragma once

class ABBBRifleEquipment;

/** 步枪Animation系统的固定调度入口 */
class FBBBRifleAnimationSystem final
{
public:
    /** @param Context	本次装备更新上下文 @return 无 */
    /** @param Equipment 卸下实例 @return 无 */
    static void Stop(ABBBRifleEquipment &Equipment);
    static void Update(ABBBRifleEquipment &Equipment);
};
