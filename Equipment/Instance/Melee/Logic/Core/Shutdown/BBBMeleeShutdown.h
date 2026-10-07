#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Shutdown/BBBEquipmentShutdown.h"
class ABBBMeleeEquipment;
/** 近战Shutdown固定入口 */
class FBBBMeleeShutdown final : public FBBBEquipmentShutdown
{
private:
    /** @param Equipment 当前装备 @return 初始化返回有效性 其它入口无返回值 */
    virtual void ShutdownInstance(ABBBEquipment &Equipment) const override;
};
