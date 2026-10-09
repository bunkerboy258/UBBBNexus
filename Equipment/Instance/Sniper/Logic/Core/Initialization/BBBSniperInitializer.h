#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Initialization/BBBEquipmentInitializer.h"

class ABBBSniperEquipment;

/** 装配狙击枪运行时数据 */
class FBBBSniperInitializer final : public FBBBEquipmentInitializer
{
private:
    /**
     * 配置狙击枪 Actor 的 Tick 时序
     * @param Equipment	目标狙击枪
     * @return 无
     */

    /**
     * 初始化狙击枪运行时数据
     * @param Equipment	待初始化的狙击枪
     * @return 是否初始化成功
     */
    virtual bool InitializeInstance(ABBBEquipment &Equipment) const override;
};
