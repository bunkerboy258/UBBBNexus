#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Update/BBBEquipmentUpdatePipeline.h"

class ABBBSMGEquipment;

/** 配置并调度冲锋枪固定更新阶段 */
class FBBBSMGUpdatePipeline final : public FBBBEquipmentUpdatePipeline
{
private:
    /**
     * 执行冲锋枪固定更新顺序
     * @param Equipment	当前冲锋枪
     * @return 无
     */
    virtual void UpdateInstance(ABBBEquipment &Equipment) const override;
};
