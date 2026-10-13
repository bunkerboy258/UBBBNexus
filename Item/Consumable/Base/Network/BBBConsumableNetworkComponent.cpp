#include "BBBWork/UBBBNexus/Item/Consumable/Base/Network/BBBConsumableNetworkComponent.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/RemoteMessage/Use/FBBBConsumableStateRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/AuthorityFact/Use/FBBBConsumableStateAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Net/UnrealNetwork.h"

UBBBConsumableNetworkComponent::UBBBConsumableNetworkComponent()
{
    SetIsReplicatedByDefault(true);
    PrimaryComponentTick.bCanEverTick = false;
}

void UBBBConsumableNetworkComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION(UBBBConsumableNetworkComponent, State, COND_SimulatedOnly);
}

void UBBBConsumableNetworkComponent::Publish(FBBBConsumableNetworkMessage Message)
{
    auto *Character = Cast<ABBBCharacter>(GetOwner());
    if (!Character || !Message.IsValid() || Character->IsNetworkMirror())
    {
        return;
    }
    if (Character->HasNetworkAuthority())
    {
        State = Message;
        Character->ForceNetUpdate();
        return;
    }
    ServerSubmitState(Message);
}

void UBBBConsumableNetworkComponent::ServerSubmitState_Implementation(FBBBConsumableNetworkMessage Message)
{
    auto *Character = Cast<ABBBCharacter>(GetOwner());
    if (!Character || !Message.IsValid() || Message.Generation < Character->GetItemGeneration()
        || Message.Generation < State.Generation
        || (Message.Generation == State.Generation && Message.Revision <= State.Revision))
    {
        return;
    }
    State = Message;
    Pending = Message;
    Character->ForceNetUpdate();
    DeliverPending();
}

void UBBBConsumableNetworkComponent::OnRep_State()
{
    Pending = State;
    DeliverPending();
}

void UBBBConsumableNetworkComponent::DeliverPending()
{
    auto *Character = Cast<ABBBCharacter>(GetOwner());
    auto *Item = Character ? Cast<ABBBConsumable>(Character->GetHeldItem()) : nullptr;
    if (!Character || !Character->IsNetworkMirror() || !Item || !Item->IsInitialized()
        || !Item->GetDefinition() || !Pending.IsValid()
        || Pending.Generation != Character->GetItemGeneration()
        || Pending.ItemId != Item->GetDefinition()->ItemId)
    {
        return;
    }
    if (DeliveredGeneration == Pending.Generation && DeliveredRevision >= Pending.Revision)
    {
        return;
    }
    if (Character->HasNetworkAuthority())
    {
        Item->SubmitInput(FBBBConsumableStateRemoteMessagePacket{{Pending}});
    }
    if (!Character->HasNetworkAuthority())
    {
        Item->SubmitInput(FBBBConsumableStateAuthorityFactPacket{{Pending}});
    }
    DeliveredGeneration = Pending.Generation;
    DeliveredRevision = Pending.Revision;
}
