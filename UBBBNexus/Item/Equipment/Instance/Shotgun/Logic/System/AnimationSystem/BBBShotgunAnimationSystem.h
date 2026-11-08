#pragma once

class ABBBShotgunEquipment;

/** 霰弹枪Animation系统的固定调度入口 */
class FBBBShotgunAnimationSystem final
{
public:
    /** @param Context	本次装备更新上下文 @return 无 */
    /** @param Equipment 卸下实例 @return 无 */
    static void Stop(ABBBShotgunEquipment &Equipment);
    static void Update(ABBBShotgunEquipment &Equipment);
};
