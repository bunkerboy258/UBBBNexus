#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Shutdown/BBBEquipmentShutdown.h"

class ABBBSniperEquipment;

/** 收束狙击枪卸下时的运行状态 */
class FBBBSniperShutdown final : public FBBBEquipmentShutdown
{
private:
    /**
     * 停止狙击枪行为与动画并清空输入
     * @param Equipment	待收束的狙击枪
     * @return 无
     */
    virtual void ShutdownInstance(ABBBEquipment &Equipment) const override;
};
