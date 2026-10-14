#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Network/BBBGrenadeNetworkComponent.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/BBBGrenadeConsumable.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"

UBBBGrenadeNetworkComponent::UBBBGrenadeNetworkComponent()
{
    SetIsReplicatedByDefault(false);
}

bool UBBBGrenadeNetworkComponent::PublishState(FBBBGrenadeNetworkMessage Message)
{
    if (!Message.IsValid())
    {
        return false;
    }
    TArray<uint8> Data;
    FMemoryWriter Writer(Data);
    uint8 Using = Message.bUsing ? 1 : 0, Released = Message.bReleased ? 1 : 0;
    Writer << Message.UseSerial << Message.ReleaseSerial << Using << Released << Message.LaunchTransform;
    return !Writer.IsError() && PublishMessage(0, MoveTemp(Data));
}

bool UBBBGrenadeNetworkComponent::ReceiveMessage(uint8 Kind, const TArray<uint8> &Data,
    uint64 Revision, bool bRemoteMessage)
{
    auto *Actor = Cast<ABBBGrenadeConsumable>(GetOwner());
    if (!Actor || Kind != 0 || Data.Num() < 10 || Data.Num() > 256)
    {
        return false;
    }
    FBBBGrenadeNetworkMessage Message;
    Message.Revision = Revision;
    uint8 Using = 0, Released = 0;
    FMemoryReader Reader(Data);
    Reader << Message.UseSerial << Message.ReleaseSerial << Using << Released << Message.LaunchTransform;
    Message.bUsing = Using != 0;
    Message.bReleased = Released != 0;
    if (Reader.IsError() || Reader.Tell() != Data.Num() || Using > 1 || Released > 1 || !Message.IsValid())
    {
        return false;
    }
    if (bRemoteMessage)
    {
        return Actor->SubmitInput(FBBBGrenadeStateRemoteMessagePacket{{}, {Message}});
    }
    return Actor->SubmitInput(FBBBGrenadeStateAuthorityFactPacket{{}, {Message}});
}
