#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/DomainData/States/BBBCharacterTraversalState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/States/BBBCharacterInteractionUseState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/States/BBBCharacterInteractionInputState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/States/BBBCharacterItemOperationState.h"
/** 角色快速近战的离散输入 */
struct FBBBCharacterMeleeBeginLocalControlPacket final
{
    /** 通知捕获的使用序号 */
    TArray<int32> Tokens;
    /** @return 格式是否有效 */
    bool IsValid() const
    {
        return !Tokens.IsEmpty();
    }
    /** @param Context 输入上下文 @return 是否允许本次输入 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return !Context.bIsMirror && Context.Life.bActionsAllowed && !Context.Traversal.bTraversing &&
               Context.InteractionUse.bBusy && Context.InteractionUse.Fact.bQuickMelee;
    }
    /** @param Context 输入状态 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.InteractionInputs.MeleeBeginTokens.Append(Tokens);
    }
};
