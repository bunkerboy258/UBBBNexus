#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Initialization/BBBEquipmentInitializer.h"

class ABBBLMGEquipment;

/** 装配轻机枪运行时数据 */
class FBBBLMGInitializer final : public FBBBEquipmentInitializer
{
private:
    /**
     * 配置轻机枪 Actor 的 Tick 时序
     * @param Equipment	目标轻机枪
     * @return 无
     */

    /**
     * 初始化轻机枪运行时数据
     * @param Equipment	待初始化的轻机枪
     * @return 是否初始化成功
     */
    virtual bool InitializeInstance(ABBBEquipment &Equipment) const override;
};
