#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Shutdown/BBBEquipmentShutdown.h"

class ABBBShotgunEquipment;

/** 收束霰弹枪卸下时的运行状态 */
class FBBBShotgunShutdown final : public FBBBEquipmentShutdown
{
private:
    /**
     * 停止霰弹枪行为与动画并清空输入
     * @param Equipment	待收束的霰弹枪
     * @return 无
     */
    virtual void ShutdownInstance(ABBBEquipment &Equipment) const override;
};
