#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Logic/Core/Update/BBBConsumableUpdatePipeline.h"
/** 具体使用生命周期 */
class FBBBHealthPackUpdatePipeline final : public FBBBConsumableUpdatePipeline
{
  protected:
    virtual void UpdateInstance(UBBBConsumable &Item) const override;
};
