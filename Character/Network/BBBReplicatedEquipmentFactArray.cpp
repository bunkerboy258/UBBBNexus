#include "BBBWork/UBBBNexus/Character/Network/BBBReplicatedEquipmentFactArray.h"

#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"

void FBBBReplicatedEquipmentFactArray::Initialize(
    UBBBCharacterNetworkComponent &InComponent)
{
    Component = &InComponent;
}

void FBBBReplicatedEquipmentFactArray::Append(FBBBEquipmentActionFact Fact)
{
    // 离散事实只负责增量投递 最终装备状态由独立复制属性收敛
    if (Items.Num() >= 128)
    {
        Items.RemoveAt(0);
        MarkArrayDirty();
    }

    FBBBReplicatedEquipmentFactItem &Item = Items.AddDefaulted_GetRef();
    Item.Fact = MoveTemp(Fact);
    MarkItemDirty(Item);
}

void FBBBReplicatedEquipmentFactArray::PostReplicatedAdd(
    const TArrayView<int32> AddedIndices,
    const int32)
{
    if (!ensureMsgf(Component, TEXT("装备事实复制数组缺少所属网络组件")))
    {
        return;
    }

    // FastArray 索引只是本机容器位置 不能作为跨端操作序号
    for (const int32 Index : AddedIndices)
    {
        if (Items.IsValidIndex(Index))
        {
            Component->SubmitEquipmentFactInput(Items[Index].Fact);
        }
    }
}
