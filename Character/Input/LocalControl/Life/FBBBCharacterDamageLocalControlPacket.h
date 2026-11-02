#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/States/BBBCharacterLifeInputState.h"

/** 已发生的独立命中 按提交顺序累计而不合并伤害 */
struct FBBBCharacterDamageLocalControlPacket final
{
    /** 本批独立作用 */
    TArray<FBBBHitContribution> Hits;

    /** @return 本批的每次贡献是否完整 */
    bool IsValid() const
    {
        return !Hits.IsEmpty() && !Hits.ContainsByPredicate([](const auto& Hit) { return !Hit.IsValid(); });
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
        Context.LifeInputs.Hits.Append(Hits);
    }
};
