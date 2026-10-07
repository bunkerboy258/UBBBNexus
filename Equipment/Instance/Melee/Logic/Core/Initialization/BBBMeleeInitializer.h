#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Initialization/BBBEquipmentInitializer.h"
class ABBBMeleeEquipment;
/** 近战Initialization固定入口 */
class FBBBMeleeInitializer final : public FBBBEquipmentInitializer
{
private:
    /** @param Equipment 当前装备 @return 初始化返回有效性 其它入口无返回值 */
    virtual bool InitializeInstance(ABBBEquipment &Equipment) const override;
    /** @param Equipment	本件装备 @return 无 配置动画后扫掠时序 */
};
