#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBCharacterDamageInboxState.h"

/** 独立命中投送消息 不包含扣血或阶段切换规则 */
struct FBBBCharacterDamageDeliveryRemoteMessagePacket final
{
    /** 本批独立作用 */
    TArray<FBBBHitContribution> Hits;
    TArray<TWeakObjectPtr<APawn>> Sources;
    TArray<uint64> Sequences;

    /** @return 本批的每次贡献是否完整 */
    bool IsValid() const
    {
        return !Hits.IsEmpty() && !Hits.ContainsByPredicate([](const auto& Hit) { return !Hit.IsValid(); }) && Sources.Num() == Hits.Num() && Sequences.Num() == Hits.Num() && !Sequences.Contains(0);
    }

    /**
     * @param Context	本次输入上下文
     * @return 是否允许应用当前输入
     */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return true;
    }

    /**
     * @param Context	本次输入上下文
     * @return 无
     */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.DamageInbox.Hits.Append(Hits);
        Context.DamageInbox.Sources.Append(Sources);
        Context.DamageInbox.Sequences.Append(Sequences);
    }
};
