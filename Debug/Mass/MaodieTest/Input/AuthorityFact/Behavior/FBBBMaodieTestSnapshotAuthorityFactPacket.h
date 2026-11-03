#pragma once
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/BehaviorSystem/DomainData/Definitions/BBBMaodieTestSnapshot.h"
/** 镜像收到的当前行为事实 */
struct FBBBMaodieTestSnapshotAuthorityFactPacket final
{
    FBBBMaodieTestSnapshot Snapshot;
    /** @return 事实是否合法 */
    bool IsValid() const
    {
        return Snapshot.Phase <= 3;
    }
    /** @param bCausal 是否为业务控制端 @return 是否可应用 */
    bool CanApply(bool bCausal) const
    {
        return !bCausal;
    }
    /** @param State 待解析状态 @return 无 */
    void Apply(FBBBMaodieTestSnapshot &State) const
    {
        State = Snapshot;
    }
};
