#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Update/BBBEquipmentUpdatePipeline.h"
class ABBBMeleeEquipment;
/** 近战Update固定入口 */
class FBBBMeleeUpdatePipeline final : public FBBBEquipmentUpdatePipeline
{
private:
    /** @param Equipment 当前装备 @return 初始化返回有效性 其它入口无返回值 */
    virtual void UpdateInstance(ABBBEquipment &Equipment) const override;
};
