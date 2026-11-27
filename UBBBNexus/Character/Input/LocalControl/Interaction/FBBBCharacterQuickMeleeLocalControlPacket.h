#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/States/BBBCharacterInteractionUseState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/States/BBBCharacterInteractionInputState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/States/BBBCharacterItemOperationState.h"
/** 角色快速近战的离散输入 */
struct FBBBCharacterQuickMeleeLocalControlPacket final
{

    /** @return 格式是否有效 */
    bool IsValid() const
    {
        return true;
    }
    /** @param Context 输入上下文 @return 是否允许本次输入 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return !Context.bIsMirror && Context.Life.bActionsAllowed &&
               !Context.InteractionUse.bBusy && Context.InteractionUse.bSwitchAllowed &&
               !Context.ItemOperations.bQuickUse && Context.Stamina.bInitialized &&
               !Context.Stamina.bLockedUntilFull && Context.Stamina.Current > 0.0f;
    }
    /** @param Context 输入状态 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.InteractionInputs.bQuickMelee = true;
    }
};
