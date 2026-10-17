#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/ParseSystem/Processors/BBBGrenadeParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/ParseSystem/DomainData/Context/BBBGrenadeParseUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/RuntimeData/BBBGrenadeRuntimeData.h"
void FBBBGrenadeParseProcessor::Update(FBBBGrenadeParseUpdateContext &Context) const
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
