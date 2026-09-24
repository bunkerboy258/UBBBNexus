#pragma once

class ABBBRifleEquipment;

/** 配置并调度步枪固定更新阶段 */
class FBBBRifleUpdatePipeline final
{
public:
    /**
     * 配置步枪 Actor 的 Tick 时序
     * @param Equipment	目标步枪
     * @return 无
     */
    static void ConfigureTick(ABBBRifleEquipment &Equipment);

    /**
     * 执行步枪固定更新顺序
     * @param Equipment	当前步枪
     * @return 无
     */
    static void Update(ABBBRifleEquipment &Equipment);
};
