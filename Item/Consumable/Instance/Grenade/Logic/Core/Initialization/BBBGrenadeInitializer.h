#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Logic/Core/Initialization/BBBConsumableInitializer.h"
class ABBBGrenadeConsumable;
/** 手雷Initializer生命周期入口 */
class FBBBGrenadeInitializer final : public FBBBConsumableInitializer
{
protected:
    /** @param Actor	所属实例 @return 初始化入口返回是否成功 其它入口无返回值 */
    bool InitializeInstance(ABBBConsumable &Actor) const override;
};
