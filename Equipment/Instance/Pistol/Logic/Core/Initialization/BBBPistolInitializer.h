#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Initialization/BBBEquipmentInitializer.h"

class ABBBPistolEquipment;

/** 装配手枪运行时数据 */
class FBBBPistolInitializer final : public FBBBEquipmentInitializer
{
private:
    /**
     * 配置手枪 Actor 的 Tick 时序
     * @param Equipment	目标手枪
     * @return 无
     */

    /**
     * 初始化手枪运行时数据
     * @param Equipment	待初始化的手枪
     * @return 是否初始化成功
     */
    virtual bool InitializeInstance(ABBBEquipment &Equipment) const override;
};
