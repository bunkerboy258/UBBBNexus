#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Update/BBBEquipmentUpdatePipeline.h"

class ABBBMinigunEquipment;

/** 配置并调度转管机枪固定更新阶段 */
class FBBBMinigunUpdatePipeline final : public FBBBEquipmentUpdatePipeline
{
private:
    /**
     * 执行转管机枪固定更新顺序
     * @param Equipment	当前转管机枪
     * @return 无
     */
    virtual void UpdateInstance(ABBBEquipment &Equipment) const override;
};
