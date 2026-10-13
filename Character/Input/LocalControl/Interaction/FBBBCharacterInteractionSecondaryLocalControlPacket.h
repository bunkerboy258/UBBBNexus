#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/States/BBBCharacterInteractionInputState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/States/BBBCharacterInteractionUseState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 当前主手物品的通用Secondary请求 */
struct FBBBCharacterInteractionSecondaryLocalControlPacket final
{
    /** @return 无数据请求始终有效 */
    bool IsValid() const
    {
        return true;
    }
    /** @param Context	角色输入上下文 @return 是否允许操作当前物品 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return !Context.bIsMirror && Context.Life.bActionsAllowed && Context.InteractionUse.bUsable;
    }
    /** @param Context	角色输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.InteractionInputs.bSecondary = true;
    }
};
