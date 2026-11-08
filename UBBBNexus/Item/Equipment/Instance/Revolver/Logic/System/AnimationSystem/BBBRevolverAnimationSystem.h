#pragma once

class ABBBRevolverEquipment;

/** 左轮Animation系统的固定调度入口 */
class FBBBRevolverAnimationSystem final
{
public:
    /** @param Context	本次装备更新上下文 @return 无 */
    /** @param Equipment 卸下实例 @return 无 */
    static void Stop(ABBBRevolverEquipment &Equipment);
    static void Update(ABBBRevolverEquipment &Equipment);
};
