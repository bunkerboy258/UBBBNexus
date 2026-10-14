#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Logic/Core/Shutdown/BBBConsumableShutdown.h"
class ABBBGrenadeConsumable;
/** 手雷Shutdown生命周期入口 */
class FBBBGrenadeShutdown final : public FBBBConsumableShutdown
{
protected:
    /** @param Actor	所属实例 @return 初始化入口返回是否成功 其它入口无返回值 */
    void ShutdownInstance(ABBBConsumable &Actor) const override;
};
