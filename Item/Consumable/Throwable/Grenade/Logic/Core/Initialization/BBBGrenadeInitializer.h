#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Logic/Core/Initialization/BBBConsumableInitializer.h"
/** 具体使用生命周期 */
class FBBBGrenadeInitializer final : public FBBBConsumableInitializer
{
  protected:
    virtual bool InitializeInstance(UBBBConsumable &Item) const override;
};
