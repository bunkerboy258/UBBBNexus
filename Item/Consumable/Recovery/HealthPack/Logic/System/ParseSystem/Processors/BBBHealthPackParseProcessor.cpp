#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/System/ParseSystem/Processors/BBBHealthPackParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/System/ParseSystem/DomainData/Context/BBBHealthPackParseUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/RuntimeData/BBBHealthPackRuntimeData.h"
void FBBBHealthPackParseProcessor::Update(FBBBHealthPackParseUpdateContext &Context) const
{
    auto &Slots = Context.Data.Parse.InputState;
    auto &Inputs = Context.Data.Use.UseInputState;
    if (Slots.Primary.bActive && Slots.Primary.Packet.IsValid() && Slots.Primary.Packet.CanApply(Inputs))
    {
        Slots.Primary.Packet.Apply(Inputs);
    }
    if (Slots.Finish.bActive && Slots.Finish.Packet.IsValid() && Slots.Finish.Packet.CanApply(Inputs))
    {
        Slots.Finish.Packet.Apply(Inputs);
    }
    Slots.Primary.bActive = false;
    Slots.Finish.bActive = false;
}
