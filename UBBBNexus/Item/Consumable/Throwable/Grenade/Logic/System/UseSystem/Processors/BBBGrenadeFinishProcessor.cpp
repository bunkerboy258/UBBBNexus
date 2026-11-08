#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/UseSystem/Processors/BBBGrenadeFinishProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/UseSystem/DomainData/Context/BBBGrenadeUseUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/RuntimeData/BBBGrenadeRuntimeData.h"
void FBBBGrenadeFinishProcessor::Update(FBBBGrenadeUseUpdateContext &Context) const
{
    auto &State = Context.Data.Use.UseState;
    auto &Inputs = Context.Data.Use.UseInputState;
    if (Inputs.bFinish)
    {
        State.bUsing = false;
    }
    Inputs.bPrimary = false;
    Inputs.bFinish = false;
}
