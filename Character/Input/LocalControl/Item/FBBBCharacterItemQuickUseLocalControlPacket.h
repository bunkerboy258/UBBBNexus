#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/States/BBBCharacterItemOperationState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/States/BBBCharacterInteractionUseState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/States/BBBCharacterInteractionInputState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 请求立即使用当前投掷物 */
struct FBBBCharacterItemQuickUseLocalControlPacket final
{
    /** @return 无数据输入始终有效 */
    bool IsValid() const
    {
        return true;
    }
    /** @param Context	角色输入上下文 @return 当前动作是否允许临时切换 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return !Context.bIsMirror && Context.Life.bActionsAllowed && !Context.RescueInputs.bBegin &&
               Context.InteractionUse.bQuickUseAllowed &&
               !Context.InteractionUse.bBusy && !Context.InteractionInputs.bQuickMelee;
    }
    /** @param Context	角色输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.ItemOperations.bQuickUse = true;
    }
};
