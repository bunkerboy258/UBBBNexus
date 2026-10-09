#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Shutdown/BBBEquipmentShutdown.h"

class ABBBLMGEquipment;

/** 收束轻机枪卸下时的运行状态 */
class FBBBLMGShutdown final : public FBBBEquipmentShutdown
{
private:
    /**
     * 停止轻机枪行为与动画并清空输入
     * @param Equipment	待收束的轻机枪
     * @return 无
     */
    virtual void ShutdownInstance(ABBBEquipment &Equipment) const override;
};
