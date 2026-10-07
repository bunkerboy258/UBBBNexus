#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Update/BBBEquipmentUpdatePipeline.h"

class ABBBRifleEquipment;

/** 配置并调度步枪固定更新阶段 */
class FBBBRifleUpdatePipeline final : public FBBBEquipmentUpdatePipeline
{
private:
    /**
     * 执行步枪固定更新顺序
     * @param Equipment	当前步枪
     * @return 无
     */
    virtual void UpdateInstance(ABBBEquipment &Equipment) const override;
};
