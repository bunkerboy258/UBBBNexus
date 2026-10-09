#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Initialization/BBBEquipmentInitializer.h"

class ABBBShotgunEquipment;

/** 装配霰弹枪运行时数据 */
class FBBBShotgunInitializer final : public FBBBEquipmentInitializer
{
private:
    /**
     * 配置霰弹枪 Actor 的 Tick 时序
     * @param Equipment	目标霰弹枪
     * @return 无
     */

    /**
     * 初始化霰弹枪运行时数据
     * @param Equipment	待初始化的霰弹枪
     * @return 是否初始化成功
     */
    virtual bool InitializeInstance(ABBBEquipment &Equipment) const override;
};
