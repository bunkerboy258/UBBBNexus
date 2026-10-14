#pragma once

class ABBBPistolEquipment;

/** 手枪Action系统的固定调度入口 */
class FBBBPistolActionSystem final
{
public:
    /** @param Context	本次装备更新上下文 @return 无 */
    static void Update(ABBBPistolEquipment &Equipment);
};
