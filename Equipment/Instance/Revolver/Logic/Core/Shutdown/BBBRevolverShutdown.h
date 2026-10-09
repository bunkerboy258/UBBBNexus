#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Shutdown/BBBEquipmentShutdown.h"

class ABBBRevolverEquipment;

/** 收束左轮卸下时的运行状态 */
class FBBBRevolverShutdown final : public FBBBEquipmentShutdown
{
private:
    /**
     * 停止左轮行为与动画并清空输入
     * @param Equipment	待收束的左轮
     * @return 无
     */
    virtual void ShutdownInstance(ABBBEquipment &Equipment) const override;
};
