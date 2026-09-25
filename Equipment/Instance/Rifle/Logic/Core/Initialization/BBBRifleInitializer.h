#pragma once

class ABBBRifleEquipment;

/** 装配步枪运行时数据 */
class FBBBRifleInitializer final
{
public:
    /**
     * 配置步枪 Actor 的 Tick 时序
     * @param Equipment	目标步枪
     * @return 无
     */
    static void ConfigureTick(ABBBRifleEquipment &Equipment);

    /**
     * 初始化步枪运行时数据
     * @param Equipment	待初始化的步枪
     * @return 是否初始化成功
     */
    static bool Initialize(ABBBRifleEquipment &Equipment);
};
