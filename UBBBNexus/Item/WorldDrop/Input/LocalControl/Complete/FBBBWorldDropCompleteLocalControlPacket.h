#pragma once
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/ParseSystem/DomainData/Context/BBBWorldDropInputContext.h"
/** 世界掉落物离散业务请求 */
struct FBBBWorldDropCompleteLocalControlPacket final
{
    /** 本次输入的全部数据 */
    TArray<FBBBWorldDropCompletion> Completions;
    /** @return 输入是否包含数据 */
    bool IsValid() const
    {
        return !Completions.IsEmpty();
    }
    /** @param Context\t本次解析上下文 @return 是否允许当前路径应用 */
    bool CanApply(const FBBBWorldDropInputContext &Context) const
    {
        return Context.bCausal;
    }
    /** @param Context\t本次解析上下文 @return 无 只写入业务黑板 */
    void Apply(FBBBWorldDropInputContext &Context) const
    {
        Context.Drop.Completions.Append(Completions);
    }
};
