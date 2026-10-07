#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Network/BBBMeleeNetworkComponent.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/BBBMeleeEquipment.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Input/RemoteMessage/Action/FBBBMeleeAttackStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Input/RemoteMessage/Action/FBBBMeleeAttackEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Input/AuthorityFact/Action/FBBBMeleeAttackStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Input/AuthorityFact/Action/FBBBMeleeAttackEndAuthorityFactPacket.h"

UBBBMeleeNetworkComponent::UBBBMeleeNetworkComponent()
{
    SetIsReplicatedByDefault(false);
}

bool UBBBMeleeNetworkComponent::PublishAttackStart(int32 Sequence)
{
    TArray<uint8> Data;
    FMemoryWriter Writer(Data);
    Writer << Sequence;
    return PublishMessage(0, MoveTemp(Data));
}

bool UBBBMeleeNetworkComponent::PublishAttackEnd(int32 Sequence)
{
    TArray<uint8> Data;
    FMemoryWriter Writer(Data);
    Writer << Sequence;
    return PublishMessage(1, MoveTemp(Data));
}

bool UBBBMeleeNetworkComponent::ReceiveMessage(
    const uint8 Kind, const TArray<uint8> &Data, const uint64 Revision, const bool bRemoteMessage)
{
    auto *Equipment = Cast<ABBBMeleeEquipment>(GetOwner());
    const int32 ExpectedSize = sizeof(int32);
    if (!Equipment || Kind >= 2 || Data.Num() != ExpectedSize)
    {
        return false;
    }

    int32 Sequence = 0;
    FMemoryReader Reader(Data);
    Reader << Sequence;
    if (Reader.IsError() || Sequence < 0)
    {
        return false;
    }

    if (Kind == 0)
    {
        if (bRemoteMessage)
        {
            return Equipment->SubmitInput(FBBBMeleeAttackStartRemoteMessagePacket{{Sequence}, {Revision}});
        }

        return Equipment->SubmitInput(FBBBMeleeAttackStartAuthorityFactPacket{{Sequence}, {Revision}});
    }

    if (Kind == 1)
    {
        if (bRemoteMessage)
        {
            return Equipment->SubmitInput(FBBBMeleeAttackEndRemoteMessagePacket{{Sequence}, {Revision}});
        }

        return Equipment->SubmitInput(FBBBMeleeAttackEndAuthorityFactPacket{{Sequence}, {Revision}});
    }

    return false;
}
