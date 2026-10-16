#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/ParseSystem/Processors/BBBGrenadeParseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/RuntimeData/BBBGrenadeRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/ParseSystem/DomainData/Context/BBBGrenadeParseUpdateContext.h"

void FBBBGrenadeParseProcessor::Update(FBBBGrenadeParseUpdateContext &Context) const
{
    const auto Process = [&Context](auto &Slot)
    {
        if (!Slot.bActive)
        {
            return;
        }
        if (Slot.Data.IsValid() && Slot.Data.CanApply(Context.bIsMirror, Context.Data.Use.UseState))
        {
            Slot.Data.Apply(Context.Data.Use.UseInputState, Context.Data.Use.UseState);
        }
        Slot.bActive = false;
        Slot.Data = {};
    };
    Process(Context.Data.Parse.InputState.HideLocalControl);
    Process(Context.Data.Parse.InputState.ShowLocalControl);
    Process(Context.Data.Parse.InputState.PrimaryLocalControl);
    Process(Context.Data.Parse.InputState.EquipLocalControl);
    Process(Context.Data.Parse.InputState.ReleaseLocalControl);
    Process(Context.Data.Parse.InputState.FinishLocalControl);
    Process(Context.Data.Parse.InputState.StateRemoteMessage);
    Process(Context.Data.Parse.InputState.StateAuthorityFact);
}

void FBBBGrenadeParseProcessor::Clear(FBBBGrenadeRuntimeData &Data)
{
    auto &Slots = Data.Parse.InputState;
    Slots.ShowLocalControl = {};
    Slots.HideLocalControl = {};
    Slots.PrimaryLocalControl.bActive = false;
    Slots.PrimaryLocalControl.Data = {};
    Slots.EquipLocalControl.bActive = false;
    Slots.EquipLocalControl.Data = {};
    Slots.ReleaseLocalControl.bActive = false;
    Slots.ReleaseLocalControl.Data = {};
    Slots.FinishLocalControl.bActive = false;
    Slots.FinishLocalControl.Data = {};
    Slots.StateRemoteMessage.bActive = false;
    Slots.StateRemoteMessage.Data = {};
    Slots.StateAuthorityFact.bActive = false;
    Slots.StateAuthorityFact.Data = {};
}
