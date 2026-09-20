#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/State/BBBNetworkFactLedgerState.h"

#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkComponent.h"

void FBBBNetworkFactLedgerState::Initialize(UBBBCharacterNetworkComponent &InComponent)
{
    Component = &InComponent;
}

void FBBBNetworkFactLedgerState::Append(FBBBEquipmentActionFact Fact)
{
    // 离散事实只用于短期增量投递 最终状态由独立复制属性收敛
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
    if (!Component)
    {
        return;
    }

    // FastArray 索引只是本机容器位置 不能把它当作跨端顺序标识
    for (const int32 Index : AddedIndices)
    {
        if (Entries.IsValidIndex(Index))
        {
            Component->SubmitEquipmentFactInput(Entries[Index].Fact);
        }
    }
}
