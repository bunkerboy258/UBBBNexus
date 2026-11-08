#pragma once

#include "BBBWork/UBBBNexus/Mass/Instance/Slime/Fragments/BBBSlimeFact.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Slime/Fragments/BBBSlimeFragment.h"

struct FBBBSlimeInputFragment;
struct FBBBSlimeFragment;

/** 已成立喷吐结果的初始输入 */
struct FBBBSlimeSpawnLocalControlPacket final
{
    using FInputFragment = FBBBSlimeInputFragment;
    FBBBSlimeFact Fact;
    TWeakObjectPtr<AActor> SourceActor;
    /** @return 出生结果是否完整 */
    bool IsValid() const { return Fact.IsValid(); }
    /** @param Current 当前粘液 @return 是否仍未初始化 */
    bool CanApply(const FBBBSlimeFragment& Current) const { return !Current.Fact.InstanceId.IsValid(); }
    /** @param Current 当前粘液 @return 仅写入初始事实 */
    void Apply(FBBBSlimeFragment& Current) const
    {
        Current.Fact = Fact;
        Current.SourceActor = SourceActor;
    }
};
