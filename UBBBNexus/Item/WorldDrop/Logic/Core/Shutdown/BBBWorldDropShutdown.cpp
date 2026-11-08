#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/Core/Shutdown/BBBWorldDropShutdown.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/BBBWorldDropItem.h"
#include "Components/SphereComponent.h"
#include "Components/MaterialBillboardComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StreamableManager.h"
void FBBBWorldDropShutdown::Shutdown(ABBBWorldDropItem &Drop)
{
    auto &Presentation = Drop.RuntimeData.Drop.PresentationState;
    if (Presentation.ResourceLoad)
    {
        Presentation.ResourceLoad->CancelHandle();
    }
    Presentation.ResourceLoad.Reset();
    Drop.SetActorTickEnabled(false);
    Drop.Body->SetSimulatePhysics(false);
    Drop.Airborne->SetVisibility(false);
    Drop.Icon->SetVisibility(false);
    Drop.Beam->SetVisibility(false);
    Drop.GroundGlow->SetVisibility(false);
    auto &State = Drop.RuntimeData.Drop.DropState;
    State.Claims.Reset();
    State.Completions.Reset();
    State.Initializations.Reset();
    State.Snapshot.Recipient = nullptr;
}
