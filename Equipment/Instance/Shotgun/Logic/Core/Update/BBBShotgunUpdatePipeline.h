#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Update/BBBEquipmentUpdatePipeline.h"

class ABBBShotgunEquipment;

/** 配置并调度霰弹枪固定更新阶段 */
class FBBBShotgunUpdatePipeline final : public FBBBEquipmentUpdatePipeline
{
private:
    /**
     * 执行霰弹枪固定更新顺序
     * @param Equipment	当前霰弹枪
     * @return 无
     */
    virtual void UpdateInstance(ABBBEquipment &Equipment) const override;
};
