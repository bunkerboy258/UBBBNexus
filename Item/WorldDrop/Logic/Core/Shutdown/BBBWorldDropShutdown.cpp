#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/Core/Shutdown/BBBWorldDropShutdown.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/BBBWorldDropItem.h"
#include "Components/SphereComponent.h"
#include "Components/MaterialBillboardComponent.h"
#include "Components/StaticMeshComponent.h"
void FBBBWorldDropShutdown::Shutdown(ABBBWorldDropItem &Drop)
{
    Drop.SetActorTickEnabled(false);
    Drop.Body->SetSimulatePhysics(false);
    Drop.Icon->SetVisibility(false);
    Drop.Beam->SetVisibility(false);
    auto &State = Drop.RuntimeData.Drop.DropState;
    State.Claims.Reset();
    State.Completions.Reset();
    State.Initializations.Reset();
    State.Snapshot.Recipient = nullptr;
}
