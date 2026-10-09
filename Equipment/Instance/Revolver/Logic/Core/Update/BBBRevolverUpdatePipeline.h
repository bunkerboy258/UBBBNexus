#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Update/BBBEquipmentUpdatePipeline.h"

class ABBBRevolverEquipment;

/** 配置并调度左轮固定更新阶段 */
class FBBBRevolverUpdatePipeline final : public FBBBEquipmentUpdatePipeline
{
private:
    /**
     * 执行左轮固定更新顺序
     * @param Equipment	当前左轮
     * @return 无
     */
    virtual void UpdateInstance(ABBBEquipment &Equipment) const override;
};
