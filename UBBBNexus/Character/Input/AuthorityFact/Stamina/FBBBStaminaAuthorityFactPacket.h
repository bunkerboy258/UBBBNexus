#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "CoreMinimal.h"

/** 镜像角色接收的当前体力结果 */
struct FBBBStaminaAuthorityFactPacket final
{
    /** 当前体力 */
    float Current = 0.0f;

    /** 当前体力上限 */
    float Maximum = 0.0f;

    /** 结果版本 */
    uint64 Revision = 0;

    /** 耗尽后锁定全部耗体力操作直至回满 */
    bool bLockedUntilFull = false;

    /** @return 包内容是否合法 */
    bool IsValid() const
    {
        return Revision > 0 && FMath::IsFinite(Current) && FMath::IsFinite(Maximum) && Current >= 0.0f
            && Maximum >= 0.0f && Current <= Maximum + KINDA_SMALL_NUMBER;
    }

    /** @param Context	本次输入上下文 @return 是否允许应用 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return Revision > Context.Stamina.Revision;
    }

    /** @param Context	本次输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.Stamina.Current = Current;
        Context.Stamina.Maximum = Maximum;
        Context.Stamina.Revision = Revision;
        Context.Stamina.bLockedUntilFull = bLockedUntilFull;
        Context.Stamina.bInitialized = true;
    }
};
