#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Network/BBBShotgunNetworkComponent.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/BBBShotgunEquipment.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/RemoteMessage/Fire/FBBBShotgunFireRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/RemoteMessage/Reload/FBBBShotgunReloadStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/RemoteMessage/Reload/FBBBShotgunReloadEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/AuthorityFact/Fire/FBBBShotgunFireAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/AuthorityFact/Reload/FBBBShotgunReloadStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/AuthorityFact/Reload/FBBBShotgunReloadEndAuthorityFactPacket.h"

UBBBShotgunNetworkComponent::UBBBShotgunNetworkComponent()
{
    SetIsReplicatedByDefault(false);
}

bool UBBBShotgunNetworkComponent::PublishFire(int32 Sequence)
{
    TArray<uint8> Data;
    FMemoryWriter Writer(Data);
    Writer << Sequence;
    return PublishMessage(0, MoveTemp(Data));
}

bool UBBBShotgunNetworkComponent::PublishReloadStart(int32 Sequence)
{
    TArray<uint8> Data;
    FMemoryWriter Writer(Data);
    Writer << Sequence;
    return PublishMessage(1, MoveTemp(Data));
}

bool UBBBShotgunNetworkComponent::PublishReloadEnd(int32 Sequence, const bool bCompleted)
{
    TArray<uint8> Data;
    FMemoryWriter Writer(Data);
    Writer << Sequence;
    uint8 Completed = bCompleted ? 1 : 0;
    Writer << Completed;
    return PublishMessage(2, MoveTemp(Data));
}

bool UBBBShotgunNetworkComponent::ReceiveMessage(
    const uint8 Kind, const TArray<uint8> &Data, const uint64 Revision, const bool bRemoteMessage)
{
    auto *Equipment = Cast<ABBBShotgunEquipment>(GetOwner());
    const int32 ExpectedSize = sizeof(int32) + (Kind == 2 ? sizeof(uint8) : 0);
    if (!Equipment || Kind >= 3 || Data.Num() != ExpectedSize)
    {
        return false;
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
            return Equipment->SubmitInput(FBBBShotgunFireRemoteMessagePacket{{Sequence}, {Revision}});
        }

        return Equipment->SubmitInput(FBBBShotgunFireAuthorityFactPacket{{Sequence}, {Revision}});
    }

    if (Kind == 1)
    {
        if (bRemoteMessage)
        {
            return Equipment->SubmitInput(FBBBShotgunReloadStartRemoteMessagePacket{{Sequence}, {Revision}});
        }

        return Equipment->SubmitInput(FBBBShotgunReloadStartAuthorityFactPacket{{Sequence}, {Revision}});
    }

    if (Kind == 2)
    {
        if (bRemoteMessage)
        {
            return Equipment->SubmitInput(FBBBShotgunReloadEndRemoteMessagePacket{{Sequence}, {Revision}, {Completed != 0}});
        }

        return Equipment->SubmitInput(FBBBShotgunReloadEndAuthorityFactPacket{{Sequence}, {Revision}, {Completed != 0}});
    }

    return false;
}
