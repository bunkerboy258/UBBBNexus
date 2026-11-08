#pragma once

class ABBBRevolverEquipment;

/** 左轮Action系统的固定调度入口 */
class FBBBRevolverActionSystem final
{
public:
    /** @param Context	本次装备更新上下文 @return 无 */
    static void Update(ABBBRevolverEquipment &Equipment);
};
