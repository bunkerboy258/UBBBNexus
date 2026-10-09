#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/ActionSystem/DomainData/States/BBBSMGActionInputState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/ActionSystem/DomainData/States/BBBSMGActionState.h"

/** 冲锋枪Action领域状态的唯一持有者 */
struct FBBBSMGActionDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBSMGActionState &ReadSMGActionState() const
    {
        return ActionState;
    }

    /** @return 本领域本帧输入状态 */
    const FBBBSMGActionInputState &ReadSMGActionInputState() const
    {
        return ActionInputState;
    }

private:
    friend class FBBBSMGActionProcessor;
    friend class FBBBSMGParseProcessor;
    /** 本领域处理器维护的状态 */
    FBBBSMGActionState ActionState;

    /** 本领域解析后等待处理的输入 */
    FBBBSMGActionInputState ActionInputState;
};
