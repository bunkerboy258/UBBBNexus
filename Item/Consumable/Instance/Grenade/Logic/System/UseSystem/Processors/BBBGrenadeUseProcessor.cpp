#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/Processors/BBBGrenadeUseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/RuntimeData/BBBGrenadeRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/DomainData/Context/BBBGrenadeUseUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/BBBGrenadeConsumable.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBGrenadeUseProcessor::Update(FBBBGrenadeUseUpdateContext &Context) const
{
    auto &State = Context.Data.Use.UseState;
    auto &Inputs = Context.Data.Use.UseInputState;
    if (Context.bIsMirror)
    {
        return;
    }

    const bool bPressed = Inputs.bPrimary && !State.bPrimaryHeld;
    State.bPrimaryHeld = Inputs.bPrimary;
    if (bPressed && !State.bHide && !State.bUsing && State.UseSerial < MAX_int32
        && Context.Character.GetHeldItem() == &Context.Actor && Context.Character.GetHeldItemQuantity() > 0)
    {
        State.bUsing = true;
        State.bReleased = false;
        ++State.UseSerial;
        UE_LOG(LogTemp, Log, TEXT("[BBBGrenade] Use started Serial=%d"), State.UseSerial);
    }
}

void FBBBGrenadeUseProcessor::Clear(FBBBGrenadeRuntimeData &Data)
{
    auto &State = Data.Use.UseState;
    State.bHide = false;
    State.bPrimaryHeld = false;
    State.bUsing = false;
    State.bReleased = false;
    auto &Inputs = Data.Use.UseInputState;
    Inputs.bPrimary = false;
    Inputs.bEquip = false;
    Inputs.bRelease = false;
    Inputs.bFinish = false;
}
