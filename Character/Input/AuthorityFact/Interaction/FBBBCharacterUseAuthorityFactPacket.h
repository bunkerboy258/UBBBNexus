#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/States/BBBCharacterInteractionUseState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/States/BBBCharacterInteractionInputState.h"
/** 已成立的角色消耗品使用结果 */
struct FBBBCharacterUseAuthorityFactPacket final
{
    /** 同帧结果按版本收敛 */
    TArray<FBBBCharacterUseFact> Snapshots;
    /** @return 是否包含结构有效的结果 */
    bool IsValid() const
    {
        for (const auto &Fact : Snapshots)
        {
            if (Fact.Revision == 0 || Fact.Serial <= 0 || Fact.ItemId.IsNone() ||
                Fact.ReleaseTransform.ContainsNaN() || Fact.InheritedVelocity.ContainsNaN())
            {
                return false;
            }
        }
        return !Snapshots.IsEmpty();
    }
    /** @param Context 解析上下文 @return 是否为镜像还原路径 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return Context.bIsMirror;
    }
    /** @param Context 角色使用结果槽 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        for (const auto &Fact : Snapshots)
        {
            if (Fact.bCommitted && !Fact.bQuickMelee)
            {
                Context.InteractionInputs.ReleaseResults.Add(Fact);
            }
            if (Fact.Revision > Context.InteractionUse.PendingFact.Revision)
            {
                Context.InteractionUse.PendingFact = Fact;
            }
        }
    }
};
