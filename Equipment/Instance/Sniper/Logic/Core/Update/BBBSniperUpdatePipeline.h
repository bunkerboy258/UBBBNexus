#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Logic/Core/Update/BBBEquipmentUpdatePipeline.h"

class ABBBSniperEquipment;

/** 配置并调度狙击枪固定更新阶段 */
class FBBBSniperUpdatePipeline final : public FBBBEquipmentUpdatePipeline
{
private:
    /**
     * 执行狙击枪固定更新顺序
     * @param Equipment	当前狙击枪
     * @return 无
     */
    virtual void UpdateInstance(ABBBEquipment &Equipment) const override;
};
