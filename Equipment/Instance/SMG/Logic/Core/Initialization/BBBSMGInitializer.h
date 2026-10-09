#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Initialization/BBBEquipmentInitializer.h"

class ABBBSMGEquipment;

/** 装配冲锋枪运行时数据 */
class FBBBSMGInitializer final : public FBBBEquipmentInitializer
{
private:
    /**
     * 配置冲锋枪 Actor 的 Tick 时序
     * @param Equipment	目标冲锋枪
     * @return 无
     */

    /**
     * 初始化冲锋枪运行时数据
     * @param Equipment	待初始化的冲锋枪
     * @return 是否初始化成功
     */
    virtual bool InitializeInstance(ABBBEquipment &Equipment) const override;
};
