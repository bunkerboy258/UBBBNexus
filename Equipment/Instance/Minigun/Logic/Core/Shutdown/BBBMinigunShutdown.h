#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Shutdown/BBBEquipmentShutdown.h"

class ABBBMinigunEquipment;

/** 收束转管机枪卸下时的运行状态 */
class FBBBMinigunShutdown final : public FBBBEquipmentShutdown
{
private:
    /**
     * 停止转管机枪行为与动画并清空输入
     * @param Equipment	待收束的转管机枪
     * @return 无
     */
    virtual void ShutdownInstance(ABBBEquipment &Equipment) const override;
};
