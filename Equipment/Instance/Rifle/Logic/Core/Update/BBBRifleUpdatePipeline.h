#pragma once

class ABBBRifleEquipment;

/** 配置并调度步枪固定更新阶段 */
class FBBBRifleUpdatePipeline final
{
public:
    /**
     * 执行步枪固定更新顺序
     * @param Equipment	当前步枪
     * @return 无
     */
    static void Update(ABBBRifleEquipment &Equipment);
};
