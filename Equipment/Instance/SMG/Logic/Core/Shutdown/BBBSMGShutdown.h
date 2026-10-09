#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Shutdown/BBBEquipmentShutdown.h"

class ABBBSMGEquipment;

/** 收束冲锋枪卸下时的运行状态 */
class FBBBSMGShutdown final : public FBBBEquipmentShutdown
{
private:
    /**
     * 停止冲锋枪行为与动画并清空输入
     * @param Equipment	待收束的冲锋枪
     * @return 无
     */
    virtual void ShutdownInstance(ABBBEquipment &Equipment) const override;
};
