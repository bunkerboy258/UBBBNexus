#pragma once
class ABBBMeleeEquipment;
/** 近战Animation领域固定调度根 */
class FBBBMeleeAnimationSystem final
{
public:
    /** @param Context 当前帧装配上下文 @return 无 */
    /** @param Equipment 卸下实例 @return 无 */
    static void Stop(ABBBMeleeEquipment &Equipment);
    static void Update(ABBBMeleeEquipment &Equipment);
};
