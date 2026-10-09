#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Shutdown/BBBEquipmentShutdown.h"

class ABBBPistolEquipment;

/** 收束手枪卸下时的运行状态 */
class FBBBPistolShutdown final : public FBBBEquipmentShutdown
{
private:
    /**
     * 停止手枪行为与动画并清空输入
     * @param Equipment	待收束的手枪
     * @return 无
     */
    virtual void ShutdownInstance(ABBBEquipment &Equipment) const override;
};
