#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Network/BBBSniperNetworkComponent.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/BBBSniperEquipment.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Input/RemoteMessage/Fire/FBBBSniperFireRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Input/RemoteMessage/Reload/FBBBSniperReloadStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Input/RemoteMessage/Reload/FBBBSniperReloadEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Input/AuthorityFact/Fire/FBBBSniperFireAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Input/AuthorityFact/Reload/FBBBSniperReloadStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Input/AuthorityFact/Reload/FBBBSniperReloadEndAuthorityFactPacket.h"

UBBBSniperNetworkComponent::UBBBSniperNetworkComponent()
{
    SetIsReplicatedByDefault(false);
}

bool UBBBSniperNetworkComponent::PublishFire(int32 Sequence)
{
    TArray<uint8> Data;
    FMemoryWriter Writer(Data);
    Writer << Sequence;
    return PublishMessage(0, MoveTemp(Data));
}

bool UBBBSniperNetworkComponent::PublishReloadStart(int32 Sequence)
{
    TArray<uint8> Data;
    FMemoryWriter Writer(Data);
    Writer << Sequence;
    return PublishMessage(1, MoveTemp(Data));
}

bool UBBBSniperNetworkComponent::PublishReloadEnd(int32 Sequence, const bool bCompleted)
{
    TArray<uint8> Data;
    FMemoryWriter Writer(Data);
    Writer << Sequence;
    uint8 Completed = bCompleted ? 1 : 0;
    Writer << Completed;
    return PublishMessage(2, MoveTemp(Data));
}

bool UBBBSniperNetworkComponent::ReceiveMessage(
    const uint8 Kind, const TArray<uint8> &Data, const uint64 Revision, const bool bRemoteMessage)
{
    auto *Equipment = Cast<ABBBSniperEquipment>(GetOwner());
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
            return Equipment->SubmitInput(FBBBSniperFireRemoteMessagePacket{{Sequence}, {Revision}});
        }

        return Equipment->SubmitInput(FBBBSniperFireAuthorityFactPacket{{Sequence}, {Revision}});
    }

    if (Kind == 1)
    {
        if (bRemoteMessage)
        {
            return Equipment->SubmitInput(FBBBSniperReloadStartRemoteMessagePacket{{Sequence}, {Revision}});
        }

        return Equipment->SubmitInput(FBBBSniperReloadStartAuthorityFactPacket{{Sequence}, {Revision}});
    }

    if (Kind == 2)
    {
        if (bRemoteMessage)
        {
            return Equipment->SubmitInput(FBBBSniperReloadEndRemoteMessagePacket{{Sequence}, {Revision}, {Completed != 0}});
        }

        return Equipment->SubmitInput(FBBBSniperReloadEndAuthorityFactPacket{{Sequence}, {Revision}, {Completed != 0}});
    }

    return false;
}
