#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/Processors/BBBCharacterPickupObservationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/BBBWorldDropItem.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Input/LocalControl/Claim/FBBBWorldDropClaimLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Input/LocalControl/Complete/FBBBWorldDropCompleteLocalControlPacket.h"
void FBBBCharacterPickupObservationProcessor::Update(FBBBCharacterNetworkUpdateContext &Context) const
{
    auto &Inbox = Context.Data.Network.PickupInboxState;
    const auto &Pickup = Context.Data.Pickup.ReadCharacterPickupState();
    auto *Character = Cast<ABBBCharacter>(Context.NetworkComponent.GetOwner());
    if (!Character)
    {
        return;
    }
    const auto Request = [&](const FBBBCharacterPickupRequest &Value)
    {
        if (IsValid(Value.Drop) && Value.Drop->GetWorld() == Character->GetWorld())
        {
            Value.Drop->SubmitInput(FBBBWorldDropClaimLocalControlPacket{{FBBBWorldDropClaim{Character, Value.DropId}}});
        }
    };
    const auto Complete = [&](const FBBBCharacterPickupReceipt &Value)
    {
        if (IsValid(Value.Grant.Drop) && Value.Grant.Drop->GetWorld() == Character->GetWorld())
        {
            Value.Grant.Drop->SubmitInput(FBBBWorldDropCompleteLocalControlPacket{{FBBBWorldDropCompletion{Character, Value}}});
        }
    };
    if (Context.NetworkIdentityState.bHasAuthority)
    {
        for (const auto &Value : Inbox.Requests)
        {
            Request(Value);
        }
        for (const auto &Value : Inbox.Receipts)
        {
            Complete(Value);
        }
    }
    Inbox.Requests.Reset();
    Inbox.Receipts.Reset();
    if (!Context.NetworkIdentityState.bLocallyControlled || Pickup.DeliverySerial == Inbox.SentSerial)
    {
        return;
    }
    Inbox.SentSerial = Pickup.DeliverySerial;
    for (const auto &Value : Pickup.Requests)
    {
        if (Context.NetworkIdentityState.bHasAuthority)
        {
            Request(Value);
        }
        if (!Context.NetworkIdentityState.bHasAuthority)
        {
            Context.NetworkComponent.ServerRequestPickup(Value);
        }
    }
    for (const auto &Value : Pickup.Receipts)
    {
        if (Context.NetworkIdentityState.bHasAuthority)
        {
            Complete(Value);
        }
        if (!Context.NetworkIdentityState.bHasAuthority)
        {
            Context.NetworkComponent.ServerCompletePickup(Value);
        }
    }
}
