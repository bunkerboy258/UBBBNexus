#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/System/UseSystem/Processors/BBBHealthPackFinishProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/System/UseSystem/DomainData/Context/BBBHealthPackUseUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/RuntimeData/BBBHealthPackRuntimeData.h"
void FBBBHealthPackFinishProcessor::Update(FBBBHealthPackUseUpdateContext &Context) const
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
