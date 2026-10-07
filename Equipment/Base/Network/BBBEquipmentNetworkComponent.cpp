#include "BBBWork/UBBBNexus/Equipment/Base/Network/BBBEquipmentNetworkComponent.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "Net/UnrealNetwork.h"

UBBBEquipmentNetworkComponent::UBBBEquipmentNetworkComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UBBBEquipmentNetworkComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION(UBBBEquipmentNetworkComponent, Messages, COND_SimulatedOnly);
}

void UBBBEquipmentNetworkComponent::StoreMessage(
    FBBBEquipmentNetworkMessage Message, TArray<FBBBEquipmentNetworkMessage> &Destination)
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
        if (Message.Revision > Existing->Revision)
        {
            *Existing = MoveTemp(Message);
        }
        return;
    }

    if (Destination.IsEmpty() || Destination[0].Generation <= Message.Generation)
    {
        Destination.Add(MoveTemp(Message));
    }
}

bool UBBBEquipmentNetworkComponent::PublishMessage(const uint8 Kind, TArray<uint8> Data)
{
    auto *Equipment = Cast<ABBBEquipment>(GetOwner());
    auto *Character = Equipment ? Cast<ABBBCharacter>(Equipment->GetOwner()) : nullptr;
    if (!Character || Character->GetActiveEquipment() != Equipment
        || Kind >= 8 || Data.IsEmpty() || Data.Num() > 32)
    {
        return false;
    }

    auto *Carrier = Character->GetEquipmentNetworkComponent();
    if (!Carrier)
    {
        return false;
    }

    const uint64 Generation = Character->GetEquipmentGeneration();
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

    FBBBEquipmentNetworkMessage Message;
    Message.EquipmentId = Equipment->GetEquipmentId();
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

void UBBBEquipmentNetworkComponent::ServerSubmitMessage_Implementation(FBBBEquipmentNetworkMessage Message)
{
    auto *Character = Cast<ABBBCharacter>(GetOwner());
    if (!Character || !Character->HasAuthority() || Message.EquipmentId.IsNone()
        || Message.Generation == 0 || Message.Revision == 0 || Message.Kind >= 8
        || Message.Data.IsEmpty() || Message.Data.Num() > 32
        || Message.Generation < Character->GetEquipmentGeneration())
    {
        return;
    }

    StoreMessage(MoveTemp(Message), RemoteMessages);
    DeliverPending();
}

void UBBBEquipmentNetworkComponent::OnRep_Messages()
{
    DeliverPending();
}

void UBBBEquipmentNetworkComponent::DeliverPending()
{
    auto *Character = Cast<ABBBCharacter>(GetOwner());
    ABBBEquipment *Equipment = Character ? Character->GetActiveEquipment() : nullptr;
    if (!Character || !Character->IsNetworkMirror() || !IsValid(Equipment) || !Equipment->IsInitialized())
    {
        return;
    }

    UBBBEquipmentNetworkComponent *Protocol = Equipment->GetNetworkComponent();
    if (!Protocol)
    {
        return;
    }

    const uint64 Generation = Character->GetEquipmentGeneration();
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
        if (Message.Generation != Generation || Message.EquipmentId != Equipment->GetEquipmentId()
            || Message.Revision <= DeliveredRevisions.FindRef(Message.Kind))
        {
            continue;
        }

        Protocol->ReceiveMessage(Message.Kind, Message.Data, Message.Revision, bRemoteMessage);
        DeliveredRevisions.Add(Message.Kind, Message.Revision);
    }
}

bool UBBBEquipmentNetworkComponent::ReceiveMessage(
    uint8 Kind, const TArray<uint8> &Data, uint64 Revision, bool bRemoteMessage)
{
    return false;
}
