#include "BBBWork/UBBBNexus/Item/Consumable/Base/Network/BBBConsumableNetworkComponent.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "Net/UnrealNetwork.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"

UBBBConsumableNetworkComponent::UBBBConsumableNetworkComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UBBBConsumableNetworkComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION(UBBBConsumableNetworkComponent, Messages, COND_SimulatedOnly);
}

void UBBBConsumableNetworkComponent::StoreMessage(
    FBBBConsumableNetworkMessage Message, TArray<FBBBConsumableNetworkMessage> &Destination)
{
    Destination.RemoveAll([&Message](const auto &Existing)
    {
        return Existing.Generation < Message.Generation;
    });
    auto *Existing = Destination.FindByPredicate([&Message](const auto &Value)
    {
        return Value.Generation == Message.Generation && Value.Kind == Message.Kind;
    });
    if (Existing)
    {
        if (Message.Revision > Existing->Revision) *Existing = MoveTemp(Message);
        return;
    }
    if (Destination.IsEmpty() || Destination[0].Generation <= Message.Generation)
    {
        Destination.Add(MoveTemp(Message));
    }
}

bool UBBBConsumableNetworkComponent::PublishMessage(const uint8 Kind, TArray<uint8> Data)
{
    auto *Consumable = Cast<ABBBConsumable>(GetOwner());
    auto *Character = Consumable ? Cast<ABBBCharacter>(Consumable->GetOwner()) : nullptr;
    if (!Character || Cast<ABBBConsumable>(Character->GetHeldItem()) != Consumable
        || Kind >= 8 || Data.IsEmpty() || Data.Num() > 256)
    {
        return false;
    }

    auto *Carrier = Character->GetConsumableNetworkComponent();
    if (!Carrier)
    {
        return false;
    }

    const uint64 Generation = Character->GetItemGeneration();
    if (Generation == 0)
    {
        return false;
    }

    if (Carrier->PublishedGeneration != Generation)
    {
        Carrier->PublishedGeneration = Generation;
        Carrier->PublishedRevision = 0;
        Carrier->Messages.Reset();
    }

    FBBBConsumableNetworkMessage Message;
    Message.ItemId = Consumable->GetDefinition()->ItemId;
    Message.Generation = Generation;
    Message.Revision = ++Carrier->PublishedRevision;
    Message.Kind = Kind;
    Message.Data = MoveTemp(Data);

    if (Character->HasNetworkAuthority())
    {
        StoreMessage(MoveTemp(Message), Carrier->Messages);
        Character->ForceNetUpdate();
        return true;
    }

    Carrier->ServerSubmitMessage(MoveTemp(Message));
    return true;
}

void UBBBConsumableNetworkComponent::ServerSubmitMessage_Implementation(FBBBConsumableNetworkMessage Message)
{
    auto *Character = Cast<ABBBCharacter>(GetOwner());
    if (!Character || !Character->HasAuthority() || Message.ItemId.IsNone()
        || Message.Generation == 0 || Message.Revision == 0 || Message.Kind >= 8
        || Message.Data.IsEmpty() || Message.Data.Num() > 256
        || Message.Generation < Character->GetItemGeneration())
    {
        return;
    }

    // 房主保存并分发拥有者已成立的当前结果 公共载体不解释具体业务
    StoreMessage(Message, Messages);
    StoreMessage(MoveTemp(Message), RemoteMessages);
    Character->ForceNetUpdate();
    DeliverPending();
}

void UBBBConsumableNetworkComponent::OnRep_Messages()
{
    DeliverPending();
}

void UBBBConsumableNetworkComponent::DeliverPending()
{
    auto *Character = Cast<ABBBCharacter>(GetOwner());
    ABBBConsumable *Consumable = Character ? Cast<ABBBConsumable>(Character->GetHeldItem()) : nullptr;
    if (!Character || !Character->IsNetworkMirror()
        || !IsValid(Consumable) || !Consumable->IsInitialized())
    {
        return;
    }

    UBBBConsumableNetworkComponent *Protocol = Consumable->GetNetworkComponent();
    if (!Protocol)
    {
        return;
    }

    const uint64 Generation = Character->GetItemGeneration();
    if (DeliveredGeneration != Generation)
    {
        DeliveredGeneration = Generation;
        DeliveredRevisions.Reset();
    }

    const bool bRemoteMessage = Character->HasNetworkAuthority();
    auto &Pending = bRemoteMessage ? RemoteMessages : Messages;
    Pending.Sort([](const auto &A, const auto &B)
    {
        return A.Revision < B.Revision;
    });

    for (const auto &Message : Pending)
    {
        if (Message.Generation != Generation
            || Message.ItemId != Consumable->GetDefinition()->ItemId
            || Message.Revision <= DeliveredRevisions.FindRef(Message.Kind))
        {
            continue;
        }

        Protocol->ReceiveMessage(Message.Kind, Message.Data, Message.Revision, bRemoteMessage);
        DeliveredRevisions.Add(Message.Kind, Message.Revision);
    }
}

bool UBBBConsumableNetworkComponent::ReceiveMessage(
    uint8 Kind, const TArray<uint8> &Data, uint64 Revision, bool bRemoteMessage)
{
    return false;
}
