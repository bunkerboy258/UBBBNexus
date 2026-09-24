#pragma once

class ABBBRifleEquipment;

/** 收束步枪卸下时的运行状态 */
class FBBBRifleShutdown final
{
public:
    /**
     * 停止步枪行为与动画并清空输入
     * @param Equipment	待收束的步枪
     * @return 无
     */
    static void Shutdown(ABBBRifleEquipment &Equipment);
};
