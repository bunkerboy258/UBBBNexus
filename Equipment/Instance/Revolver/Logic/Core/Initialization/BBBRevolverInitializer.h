#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Initialization/BBBEquipmentInitializer.h"

class ABBBRevolverEquipment;

/** 装配左轮运行时数据 */
class FBBBRevolverInitializer final : public FBBBEquipmentInitializer
{
private:
    /**
     * 配置左轮 Actor 的 Tick 时序
     * @param Equipment	目标左轮
     * @return 无
     */

    /**
     * 初始化左轮运行时数据
     * @param Equipment	待初始化的左轮
     * @return 是否初始化成功
     */
    virtual bool InitializeInstance(ABBBEquipment &Equipment) const override;
};
