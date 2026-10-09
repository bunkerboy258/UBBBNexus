#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Initialization/BBBEquipmentInitializer.h"

class ABBBMinigunEquipment;

/** 装配转管机枪运行时数据 */
class FBBBMinigunInitializer final : public FBBBEquipmentInitializer
{
private:
    /**
     * 配置转管机枪 Actor 的 Tick 时序
     * @param Equipment	目标转管机枪
     * @return 无
     */

    /**
     * 初始化转管机枪运行时数据
     * @param Equipment	待初始化的转管机枪
     * @return 是否初始化成功
     */
    virtual bool InitializeInstance(ABBBEquipment &Equipment) const override;
};
