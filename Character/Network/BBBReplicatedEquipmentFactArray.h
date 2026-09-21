#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipmentActionFact.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "BBBReplicatedEquipmentFactArray.generated.h"

class UBBBCharacterNetworkComponent;

/** 装备事实增量数组中的单个 UE 复制条目 */
USTRUCT()
struct FBBBReplicatedEquipmentFactItem final : public FFastArraySerializerItem
{
    GENERATED_BODY()

    /** 由接收边界翻译为领域输入包的装备事实 */
    UPROPERTY()
    FBBBEquipmentActionFact Fact;
};

/** 权威向模拟代理增量发送装备事实的 UE 复制数组 */
USTRUCT()
struct FBBBReplicatedEquipmentFactArray final : public FFastArraySerializer
{
    GENERATED_BODY()

    /**
     * 绑定复制数组所属组件
     * @param InComponent 持有复制属性的网络组件
     * @return 无
     */
    void Initialize(UBBBCharacterNetworkComponent &InComponent);

    /**
     * 追加权威已经接受的装备事实
     * @param Fact 需要复制的装备事实
     * @return 无
     */
    void Append(FBBBEquipmentActionFact Fact);

    /**
     * 将新增条目交给网络组件输入边界
     * @param AddedIndices 本次新增条目的本地索引
     * @param FinalSize 本次复制后的数组长度
     * @return 无
     */
    void PostReplicatedAdd(TArrayView<int32> AddedIndices, int32 FinalSize);

    /**
     * 序列化装备事实增量
     * @param DeltaParams UE 增量序列化参数
     * @return 序列化是否成功
     */
    bool NetDeltaSerialize(FNetDeltaSerializeInfo &DeltaParams)
    {
        return FastArrayDeltaSerialize<FBBBReplicatedEquipmentFactItem, FBBBReplicatedEquipmentFactArray>(
            Items,
            DeltaParams,
            *this);
    }

private:
    /** 最近的一百二十八条装备事实 */
    UPROPERTY()
    TArray<FBBBReplicatedEquipmentFactItem> Items;

    /** 生命周期由拥有该数组的网络组件保证 */
    UBBBCharacterNetworkComponent *Component = nullptr;
};

template<>
struct TStructOpsTypeTraits<FBBBReplicatedEquipmentFactArray>
    : public TStructOpsTypeTraitsBase2<FBBBReplicatedEquipmentFactArray>
{
    enum
    {
        WithNetDeltaSerializer = true
    };
};
