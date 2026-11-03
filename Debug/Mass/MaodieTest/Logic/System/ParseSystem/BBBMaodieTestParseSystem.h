#pragma once
#include <type_traits>
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Input/AuthorityFact/Behavior/FBBBMaodieTestSnapshotAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/RuntimeData/BBBMaodieTestRuntimeData.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/ParseSystem/Processors/BBBMaodieTestParseProcessor.h"
/** 耄耋Parse调度根 */
class FBBBMaodieTestParseSystem final
{
public:
    /** @param Data 黑板 @param Packet 最新事实 @return 无 只贡献固定槽位 */
    template<typename TPacket>
    static void Submit(FBBBMaodieTestRuntimeData &Data, TPacket &&Packet)
    {
        static_assert(std::is_same_v<std::decay_t<TPacket>, FBBBMaodieTestSnapshotAuthorityFactPacket>);
        Data.Parse.State.Pending = Packet.Snapshot;
        Data.Parse.State.bActive = true;
    }

    /** @param Actor 耄耋 @param DeltaSeconds 帧间隔 @param bCausal 业务控制身份 @return 无 */
    static void Update(ABBBMaodieTest &Actor, float DeltaSeconds, bool bCausal)
    {
        FBBBMaodieTestParseProcessor::Update(Actor, DeltaSeconds, bCausal);
    }
};
