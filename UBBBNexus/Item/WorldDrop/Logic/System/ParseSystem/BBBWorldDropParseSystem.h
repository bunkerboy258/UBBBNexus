#pragma once
#include <type_traits>
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/RuntimeData/BBBWorldDropRuntimeData.h"
/** 世界掉落物固定输入队列的提交与解析入口 */
class FBBBWorldDropParseSystem final
{
public:
    /** @param Data\t唯一运行时黑板 @param Packet\t待贡献的输入包 @return 无 */
    template<typename TPacket>
    static void Submit(FBBBWorldDropRuntimeData &Data, TPacket &&Packet)
    {
        using FPacket = typename TDecay<TPacket>::Type;
        auto &Slots = Data.Parse.InputState;
        if constexpr (std::is_same_v<FPacket, FBBBWorldDropInitializeLocalControlPacket>)
        {
            Slots.Initialize.Data.Initializations.Append(Packet.Initializations);
            Slots.Initialize.bActive = true;
        }
        if constexpr (std::is_same_v<FPacket, FBBBWorldDropClaimLocalControlPacket>)
        {
            Slots.Claim.Data.Claims.Append(Packet.Claims);
            Slots.Claim.bActive = true;
        }
        if constexpr (std::is_same_v<FPacket, FBBBWorldDropCompleteLocalControlPacket>)
        {
            Slots.Complete.Data.Completions.Append(Packet.Completions);
            Slots.Complete.bActive = true;
        }
        if constexpr (std::is_same_v<FPacket, FBBBWorldDropSnapshotAuthorityFactPacket>)
        {
            Slots.Snapshot.Data.Snapshots.Append(Packet.Snapshots);
            Slots.Snapshot.bActive = true;
        }
    }
    /** @param Data\t唯一运行时黑板 @param bCausal\t是否执行业务因果 @return 无 */
    void Update(FBBBWorldDropRuntimeData &Data, bool bCausal) const;
};
