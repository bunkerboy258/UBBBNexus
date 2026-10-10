#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Network/BBBMinigunNetworkComponent.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/BBBMinigunEquipment.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/RemoteMessage/Fire/FBBBMinigunSpinRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/AuthorityFact/Fire/FBBBMinigunSpinAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/RemoteMessage/Fire/FBBBMinigunFireRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/RemoteMessage/Reload/FBBBMinigunReloadStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/RemoteMessage/Reload/FBBBMinigunReloadEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/AuthorityFact/Fire/FBBBMinigunFireAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/AuthorityFact/Reload/FBBBMinigunReloadStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/AuthorityFact/Reload/FBBBMinigunReloadEndAuthorityFactPacket.h"

UBBBMinigunNetworkComponent::UBBBMinigunNetworkComponent()
{
    SetIsReplicatedByDefault(false);
}

bool UBBBMinigunNetworkComponent::PublishFire(int32 Sequence)
{
    TArray<uint8> Data;
    FMemoryWriter Writer(Data);
    Writer << Sequence;
    return PublishMessage(0, MoveTemp(Data));
}

bool UBBBMinigunNetworkComponent::PublishSpin(const bool bSpinning)
{
    TArray<uint8> Data;
    FMemoryWriter Writer(Data);
    uint8 Spinning = bSpinning ? 1 : 0;
    Writer << Spinning;
    return PublishMessage(3, MoveTemp(Data));
}

bool UBBBMinigunNetworkComponent::PublishReloadStart(int32 Sequence)
{
    TArray<uint8> Data;
    FMemoryWriter Writer(Data);
    Writer << Sequence;
    return PublishMessage(1, MoveTemp(Data));
}

bool UBBBMinigunNetworkComponent::PublishReloadEnd(int32 Sequence, const bool bCompleted)
{
    TArray<uint8> Data;
    FMemoryWriter Writer(Data);
    Writer << Sequence;
    uint8 Completed = bCompleted ? 1 : 0;
    Writer << Completed;
    return PublishMessage(2, MoveTemp(Data));
}

bool UBBBMinigunNetworkComponent::ReceiveMessage(
    const uint8 Kind, const TArray<uint8> &Data, const uint64 Revision, const bool bRemoteMessage)
{
    auto *Equipment = Cast<ABBBMinigunEquipment>(GetOwner());
    const int32 ExpectedSize = Kind == 3 ? sizeof(uint8) : sizeof(int32) + (Kind == 2 ? sizeof(uint8) : 0);
    if (!Equipment || Kind >= 4 || Data.Num() != ExpectedSize)
    {
        return false;
    }

    if (Kind == 3)
    {
        const uint8 Spinning = Data[0];
        if (Spinning > 1)
        {
            return false;
        }
        if (bRemoteMessage)
        {
            return Equipment->SubmitInput(FBBBMinigunSpinRemoteMessagePacket{{Spinning}, {Revision}});
        }
        return Equipment->SubmitInput(FBBBMinigunSpinAuthorityFactPacket{{Spinning}, {Revision}});
    }

    int32 Sequence = 0;
    uint8 Completed = 0;
    FMemoryReader Reader(Data);
    Reader << Sequence;
    if (Kind == 2)
    {
        Reader << Completed;
    }
    if (Reader.IsError() || Sequence < 0 || Completed > 1)
    {
        return false;
    }

    if (Kind == 0)
    {
        if (bRemoteMessage)
        {
            return Equipment->SubmitInput(FBBBMinigunFireRemoteMessagePacket{{Sequence}, {Revision}});
        }

        return Equipment->SubmitInput(FBBBMinigunFireAuthorityFactPacket{{Sequence}, {Revision}});
    }

    if (Kind == 1)
    {
        if (bRemoteMessage)
        {
            return Equipment->SubmitInput(FBBBMinigunReloadStartRemoteMessagePacket{{Sequence}, {Revision}});
        }

        return Equipment->SubmitInput(FBBBMinigunReloadStartAuthorityFactPacket{{Sequence}, {Revision}});
    }

    if (Kind == 2)
    {
        if (bRemoteMessage)
        {
            return Equipment->SubmitInput(FBBBMinigunReloadEndRemoteMessagePacket{{Sequence}, {Revision}, {Completed != 0}});
        }

        return Equipment->SubmitInput(FBBBMinigunReloadEndAuthorityFactPacket{{Sequence}, {Revision}, {Completed != 0}});
    }

    return false;
}
