#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Update/BBBEquipmentUpdatePipeline.h"

class ABBBPistolEquipment;

/** 配置并调度手枪固定更新阶段 */
class FBBBPistolUpdatePipeline final : public FBBBEquipmentUpdatePipeline
{
private:
    /**
     * 执行手枪固定更新顺序
     * @param Equipment	当前手枪
     * @return 无
     */
    virtual void UpdateInstance(ABBBEquipment &Equipment) const override;
};
