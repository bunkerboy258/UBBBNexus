#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/DomainData/States/BBBCharacterTraversalState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/States/BBBCharacterInteractionInputState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/States/BBBCharacterInteractionUseState.h"
/** 角色离散使用请求 */
struct FBBBCharacterConsumableStageLocalControlPacket final
{
    /** 按输入到达顺序累计的动作阶段 */
    TArray<uint8> Stages;
    /** @return 格式有效 */
    bool IsValid() const
    {
        return !Stages.IsEmpty();
    }
    /** @param Context 只读解析上下文 @return 本次请求是否允许 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return !Context.bIsMirror && Context.Life.bActionsAllowed && Context.InteractionUse.bBusy &&
               Context.InteractionUse.Fact.Stage >= 1 && Context.InteractionUse.Fact.Stage <= 3;
    }
    /** @param Context 输入状态 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.InteractionInputs.UseStages.Append(Stages);
    }
};
