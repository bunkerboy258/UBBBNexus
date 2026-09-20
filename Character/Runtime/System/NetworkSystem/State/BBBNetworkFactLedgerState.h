#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Definition/BBBNetworkFactEntryDefinition.h"
#include "BBBNetworkFactLedgerState.generated.h"

class UBBBCharacterNetworkComponent;

USTRUCT()
struct FBBBNetworkFactLedgerState final : public FFastArraySerializer
{
    GENERATED_BODY()

    void Initialize(UBBBCharacterNetworkComponent &InComponent);

    void Append(FBBBEquipmentActionFact Fact);

    void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);

    bool NetDeltaSerialize(FNetDeltaSerializeInfo &DeltaParams)
    {
        return FastArrayDeltaSerialize<FBBBNetworkFactEntryDefinition, FBBBNetworkFactLedgerState>(
            Entries,
            DeltaParams,
            *this);
    }

    UPROPERTY()
    TArray<FBBBNetworkFactEntryDefinition> Entries;

private:
    UBBBCharacterNetworkComponent *Component = nullptr;
};

template<>
struct TStructOpsTypeTraits<FBBBNetworkFactLedgerState> : public TStructOpsTypeTraitsBase2<FBBBNetworkFactLedgerState>
{
    enum
    {
        WithNetDeltaSerializer = true
    };
};
