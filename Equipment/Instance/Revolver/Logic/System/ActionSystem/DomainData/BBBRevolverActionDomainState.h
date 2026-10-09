#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/ActionSystem/DomainData/States/BBBRevolverActionInputState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/ActionSystem/DomainData/States/BBBRevolverActionState.h"

/** 左轮Action领域状态的唯一持有者 */
struct FBBBRevolverActionDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBRevolverActionState &ReadRevolverActionState() const
    {
        return ActionState;
    }

    /** @return 本领域本帧输入状态 */
    const FBBBRevolverActionInputState &ReadRevolverActionInputState() const
    {
        return ActionInputState;
    }

private:
    friend class FBBBRevolverActionProcessor;
    friend class FBBBRevolverParseProcessor;
    /** 本领域处理器维护的状态 */
    FBBBRevolverActionState ActionState;

    /** 本领域解析后等待处理的输入 */
    FBBBRevolverActionInputState ActionInputState;
};
