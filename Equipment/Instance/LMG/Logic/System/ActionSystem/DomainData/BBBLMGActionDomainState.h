#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/ActionSystem/DomainData/States/BBBLMGActionInputState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/ActionSystem/DomainData/States/BBBLMGActionState.h"

/** 轻机枪Action领域状态的唯一持有者 */
struct FBBBLMGActionDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBLMGActionState &ReadLMGActionState() const
    {
        return ActionState;
    }

    /** @return 本领域本帧输入状态 */
    const FBBBLMGActionInputState &ReadLMGActionInputState() const
    {
        return ActionInputState;
    }

private:
    friend class FBBBLMGActionProcessor;
    friend class FBBBLMGParseProcessor;
    /** 本领域处理器维护的状态 */
    FBBBLMGActionState ActionState;

    /** 本领域解析后等待处理的输入 */
    FBBBLMGActionInputState ActionInputState;
};
