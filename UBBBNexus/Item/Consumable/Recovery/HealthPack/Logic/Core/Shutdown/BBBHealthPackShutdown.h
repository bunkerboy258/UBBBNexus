#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Logic/Core/Shutdown/BBBConsumableShutdown.h"
/** 具体使用生命周期 */
class FBBBHealthPackShutdown final : public FBBBConsumableShutdown
{
  protected:
    virtual void ShutdownInstance(UBBBConsumable &Item) const override;
};
