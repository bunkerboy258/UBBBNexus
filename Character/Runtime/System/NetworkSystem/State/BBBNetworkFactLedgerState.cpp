#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/State/BBBNetworkFactLedgerState.h"

#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkComponent.h"

void FBBBNetworkFactLedgerState::Initialize(UBBBCharacterNetworkComponent &InComponent)
{
    Component = &InComponent;
}

void FBBBNetworkFactLedgerState::Append(FBBBEquipmentActionFact Fact)
{
    if (Entries.Num() >= 128)
    {
        Entries.RemoveAt(0);
        MarkArrayDirty();
    }

    FBBBNetworkFactEntryDefinition &Entry = Entries.AddDefaulted_GetRef();
    Entry.Fact = MoveTemp(Fact);
    MarkItemDirty(Entry);
}

void FBBBNetworkFactLedgerState::PostReplicatedAdd(
    const TArrayView<int32> AddedIndices,
    const int32 FinalSize)
{
    if (!ensureMsgf(Component, TEXT("[UBBBC]Network fact ledger received entries without a component")))
    {
        return;
    }

    for (const int32 Index : AddedIndices)
    {
        if (ensureMsgf(Entries.IsValidIndex(Index), TEXT("[UBBBC]Network fact ledger index is invalid")))
        {
            Component->DeliverEquipmentFact(Entries[Index].Fact);
        }
    }
}
