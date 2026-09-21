#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBNetworkFactEntry.h"
#include "BBBNetworkFactLedger.generated.h"

class UBBBCharacterNetworkComponent;

USTRUCT()
/**
 * 有界的离散事实复制账本
 *
 * 账本由权威组件拥有 接收回调不接触黑板
 * 仅把条目交给组件的输入投递边界
 */
struct FBBBNetworkFactLedger final : public FFastArraySerializer
{
    GENERATED_BODY()

    /**
     * 绑定账本所属组件
     *
     * @param InComponent	持有复制属性的网络组件
     */
    void Initialize(UBBBCharacterNetworkComponent &InComponent);

    /**
     * 追加权威已经接受的离散事实
     *
     * @param Fact	需要复制的最终事实
     */
    void Append(FBBBEquipmentActionFact Fact);

    /**
     * 将新增条目转交网络组件的输入边界
     *
     * @param AddedIndices	本次新增条目的本地索引
     * @param FinalSize	本次复制后的账本大小
     */
    void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);

    bool NetDeltaSerialize(FNetDeltaSerializeInfo &DeltaParams)
    {
        return FastArrayDeltaSerialize<FBBBNetworkFactEntry, FBBBNetworkFactLedger>(
            Entries,
            DeltaParams,
            *this);
    }

    UPROPERTY()
    /** 最近的一百二十八条离散事实 超出上限时移除最旧条目 */
    TArray<FBBBNetworkFactEntry> Entries;

    /** 不参与复制 生命周期由拥有该账本的组件保证 */
    UBBBCharacterNetworkComponent *Component = nullptr;
};

template<>
struct TStructOpsTypeTraits<FBBBNetworkFactLedger> : public TStructOpsTypeTraitsBase2<FBBBNetworkFactLedger>
{
    enum
    {
        WithNetDeltaSerializer = true
    };
};
