#pragma once
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/ParseSystem/DomainData/Context/BBBWorldDropInputContext.h"
/** 世界掉落物当前结果还原 */
struct FBBBWorldDropSnapshotAuthorityFactPacket final
{
    /** 本次输入的全部数据 */
    TArray<FBBBWorldDropSnapshot> Snapshots;
    /** @return 输入是否包含数据 */
    bool IsValid() const
    {
        return !Snapshots.IsEmpty();
    }
    /** @param Context\t本次解析上下文 @return 是否允许当前路径应用 */
    bool CanApply(const FBBBWorldDropInputContext &Context) const
    {
        return !Context.bCausal;
    }
    /** @param Context\t本次解析上下文 @return 无 只写入业务黑板 */
    void Apply(FBBBWorldDropInputContext &Context) const
    {
        for (const auto &Snapshot : Snapshots)
        {
            if (Snapshot.Revision > Context.Drop.Snapshot.Revision)
            {
                Context.Drop.Snapshot = Snapshot;
            }
        }
    }
};
