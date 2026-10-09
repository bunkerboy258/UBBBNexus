#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Update/BBBEquipmentUpdatePipeline.h"

class ABBBLMGEquipment;

/** 配置并调度轻机枪固定更新阶段 */
class FBBBLMGUpdatePipeline final : public FBBBEquipmentUpdatePipeline
{
private:
    /**
     * 执行轻机枪固定更新顺序
     * @param Equipment	当前轻机枪
     * @return 无
     */
    virtual void UpdateInstance(ABBBEquipment &Equipment) const override;
};
