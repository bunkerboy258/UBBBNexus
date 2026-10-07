#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Network/BBBRifleNetworkComponent.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/RemoteMessage/Fire/FBBBRifleFireRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/RemoteMessage/Reload/FBBBRifleReloadStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/RemoteMessage/Reload/FBBBRifleReloadEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/AuthorityFact/Fire/FBBBRifleFireAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/AuthorityFact/Reload/FBBBRifleReloadStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/AuthorityFact/Reload/FBBBRifleReloadEndAuthorityFactPacket.h"

UBBBRifleNetworkComponent::UBBBRifleNetworkComponent()
{
    SetIsReplicatedByDefault(false);
}

bool UBBBRifleNetworkComponent::PublishFire(int32 Sequence)
{
    TArray<uint8> Data;
    FMemoryWriter Writer(Data);
    Writer << Sequence;
    return PublishMessage(0, MoveTemp(Data));
}

bool UBBBRifleNetworkComponent::PublishReloadStart(int32 Sequence)
{
    TArray<uint8> Data;
    FMemoryWriter Writer(Data);
    Writer << Sequence;
    return PublishMessage(1, MoveTemp(Data));
}

bool UBBBRifleNetworkComponent::PublishReloadEnd(int32 Sequence, const bool bCompleted)
{
    TArray<uint8> Data;
    FMemoryWriter Writer(Data);
    Writer << Sequence;
    uint8 Completed = bCompleted ? 1 : 0;
    Writer << Completed;
    return PublishMessage(2, MoveTemp(Data));
}

bool UBBBRifleNetworkComponent::ReceiveMessage(
    const uint8 Kind, const TArray<uint8> &Data, const uint64 Revision, const bool bRemoteMessage)
{
    auto *Equipment = Cast<ABBBRifleEquipment>(GetOwner());
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
            return Equipment->SubmitInput(FBBBRifleFireRemoteMessagePacket{{Sequence}, {Revision}});
        }

        return Equipment->SubmitInput(FBBBRifleFireAuthorityFactPacket{{Sequence}, {Revision}});
    }

    if (Kind == 1)
    {
        if (bRemoteMessage)
        {
            return Equipment->SubmitInput(FBBBRifleReloadStartRemoteMessagePacket{{Sequence}, {Revision}});
        }

        return Equipment->SubmitInput(FBBBRifleReloadStartAuthorityFactPacket{{Sequence}, {Revision}});
    }

    if (Kind == 2)
    {
        if (bRemoteMessage)
        {
            return Equipment->SubmitInput(FBBBRifleReloadEndRemoteMessagePacket{{Sequence}, {Revision}, {Completed != 0}});
        }

        return Equipment->SubmitInput(FBBBRifleReloadEndAuthorityFactPacket{{Sequence}, {Revision}, {Completed != 0}});
    }

    return false;
}
