#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Logic/Core/Update/BBBConsumableUpdatePipeline.h"
class ABBBGrenadeConsumable;
/** 手雷UpdatePipeline生命周期入口 */
class FBBBGrenadeUpdatePipeline final : public FBBBConsumableUpdatePipeline
{
protected:
    /** @param Actor	所属实例 @return 初始化入口返回是否成功 其它入口无返回值 */
    void UpdateInstance(ABBBConsumable &Actor) const override;
};
