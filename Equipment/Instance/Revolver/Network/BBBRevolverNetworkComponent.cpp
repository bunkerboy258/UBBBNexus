#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Network/BBBRevolverNetworkComponent.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/BBBRevolverEquipment.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Input/RemoteMessage/Fire/FBBBRevolverFireRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Input/RemoteMessage/Reload/FBBBRevolverReloadStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Input/RemoteMessage/Reload/FBBBRevolverReloadEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Input/AuthorityFact/Fire/FBBBRevolverFireAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Input/AuthorityFact/Reload/FBBBRevolverReloadStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Input/AuthorityFact/Reload/FBBBRevolverReloadEndAuthorityFactPacket.h"

UBBBRevolverNetworkComponent::UBBBRevolverNetworkComponent()
{
    SetIsReplicatedByDefault(false);
}

bool UBBBRevolverNetworkComponent::PublishFire(int32 Sequence)
{
    TArray<uint8> Data;
    FMemoryWriter Writer(Data);
    Writer << Sequence;
    return PublishMessage(0, MoveTemp(Data));
}

bool UBBBRevolverNetworkComponent::PublishReloadStart(int32 Sequence)
{
    TArray<uint8> Data;
    FMemoryWriter Writer(Data);
    Writer << Sequence;
    return PublishMessage(1, MoveTemp(Data));
}

bool UBBBRevolverNetworkComponent::PublishReloadEnd(int32 Sequence, const bool bCompleted)
{
    TArray<uint8> Data;
    FMemoryWriter Writer(Data);
    Writer << Sequence;
    uint8 Completed = bCompleted ? 1 : 0;
    Writer << Completed;
    return PublishMessage(2, MoveTemp(Data));
}

bool UBBBRevolverNetworkComponent::ReceiveMessage(
    const uint8 Kind, const TArray<uint8> &Data, const uint64 Revision, const bool bRemoteMessage)
{
    auto *Equipment = Cast<ABBBRevolverEquipment>(GetOwner());
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
            return Equipment->SubmitInput(FBBBRevolverFireRemoteMessagePacket{{Sequence}, {Revision}});
        }

        return Equipment->SubmitInput(FBBBRevolverFireAuthorityFactPacket{{Sequence}, {Revision}});
    }

    if (Kind == 1)
    {
        if (bRemoteMessage)
        {
            return Equipment->SubmitInput(FBBBRevolverReloadStartRemoteMessagePacket{{Sequence}, {Revision}});
        }

        return Equipment->SubmitInput(FBBBRevolverReloadStartAuthorityFactPacket{{Sequence}, {Revision}});
    }

    if (Kind == 2)
    {
        if (bRemoteMessage)
        {
            return Equipment->SubmitInput(FBBBRevolverReloadEndRemoteMessagePacket{{Sequence}, {Revision}, {Completed != 0}});
        }

        return Equipment->SubmitInput(FBBBRevolverReloadEndAuthorityFactPacket{{Sequence}, {Revision}, {Completed != 0}});
    }

    return false;
}
