#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Shutdown/BBBEquipmentShutdown.h"

class ABBBRifleEquipment;

/** 收束步枪卸下时的运行状态 */
class FBBBRifleShutdown final : public FBBBEquipmentShutdown
{
private:
    /**
     * 停止步枪行为与动画并清空输入
     * @param Equipment	待收束的步枪
     * @return 无
     */
    virtual void ShutdownInstance(ABBBEquipment &Equipment) const override;
};
