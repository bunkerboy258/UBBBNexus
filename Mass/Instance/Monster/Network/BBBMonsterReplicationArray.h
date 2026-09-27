#pragma once

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Network/BBBMonsterReplicationItem.h"
#include "BBBMonsterReplicationArray.generated.h"

/** 原生增量复制容器 */
USTRUCT()
struct FBBBMonsterReplicationArray final : public FFastArraySerializer
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FBBBMonsterReplicationItem> Items;

    bool NetDeltaSerialize(FNetDeltaSerializeInfo& Delta)
    {
        return FastArrayDeltaSerialize<FBBBMonsterReplicationItem, FBBBMonsterReplicationArray>(Items, Delta, *this);
    }
};

template<>
struct TStructOpsTypeTraits<FBBBMonsterReplicationArray> : TStructOpsTypeTraitsBase2<FBBBMonsterReplicationArray>
{
    enum
    {
        WithNetDeltaSerializer = true
    };
};
