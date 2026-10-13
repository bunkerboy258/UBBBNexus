#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/Processors/BBBGrenadeFinishProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/DomainData/Context/BBBGrenadeUseUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/BBBGrenadeConsumable.h"
#include "Components/StaticMeshComponent.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBGrenadeFinishProcessor::Update(FBBBGrenadeUseUpdateContext &Context) const
{
    auto &State = Context.Data.Use.UseState;
    auto &Inputs = Context.Data.Use.UseInputState;
    if (!Context.bIsMirror && (Inputs.bFinish || !State.bAllowed))
    {
        State.bUsing = false;
        State.bReleased = false;
    }
    Context.Actor.GetMesh()->SetVisibility(!State.bReleased, true);
    Inputs.bPrimary = false;
    Inputs.bEquip = false;
    Inputs.bRelease = false;
    Inputs.bFinish = false;
    Inputs.bHasPermission = false;
    if (!State.bAllowed || Context.Character.GetHeldItem() != &Context.Actor)
    {
        Context.Actor.SetActorTickEnabled(false);
    }
}
