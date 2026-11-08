#pragma once

class ABBBLMGEquipment;

/** 轻机枪Parse系统的固定调度入口 */
class FBBBLMGParseSystem final
{
public:
    /** @param Context	本次装备更新上下文 @return 无 */
    static void Update(ABBBLMGEquipment &Equipment);
};
