#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/NetworkSystem/Processors/BBBWorldDropNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/NetworkSystem/DomainData/Context/BBBWorldDropNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/BBBWorldDropItem.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Network/BBBWorldDropNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Pickup/FBBBCharacterPickupGrantLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/BBBItemDefinition.h"
#include "Engine/World.h"
void FBBBWorldDropNetworkProcessor::Update(FBBBWorldDropNetworkUpdateContext &Context) const
{
    auto &Drop = Context.Drop;
    if (!Drop.HasAuthority() || !Drop.GetWorld()->IsGameWorld())
    {
        return;
    }
    const auto &Snapshot = Context.Data.Drop.ReadWorldDropState().Snapshot;
    auto &Network = Context.Data.Network.NetworkState;
    if (Snapshot.Revision > Network.PublishedRevision)
    {
        Drop.Network->Publish(Snapshot);
        Network.PublishedRevision = Snapshot.Revision;
    }
    if (Snapshot.bClaimed && !Snapshot.bConsumed && IsValid(Snapshot.Recipient)
        && Snapshot.Definition && Snapshot.Operation > Network.GrantedOperation)
    {
        FBBBCharacterPickupGrant Grant;
        Grant.Drop = &Drop;
        Grant.DropId = Snapshot.DropId;
        Grant.Operation = Snapshot.Operation;
        Grant.ItemId = Snapshot.Definition->ItemId;
        Grant.Quantity = Snapshot.Quantity;
        if (Snapshot.Recipient->IsLocallyControlled())
        {
            Snapshot.Recipient->SubmitInput(FBBBCharacterPickupGrantLocalControlPacket{{Grant}});
        }
        if (!Snapshot.Recipient->IsLocallyControlled())
        {
            Snapshot.Recipient->GetCharacterNetworkComponent()->ClientGrantPickup(Grant);
        }
        Network.GrantedOperation = Snapshot.Operation;
    }
}
