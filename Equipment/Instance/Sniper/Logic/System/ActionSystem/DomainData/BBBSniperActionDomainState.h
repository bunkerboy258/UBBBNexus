#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/ActionSystem/DomainData/States/BBBSniperActionInputState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/ActionSystem/DomainData/States/BBBSniperActionState.h"

/** 狙击枪Action领域状态的唯一持有者 */
struct FBBBSniperActionDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBSniperActionState &ReadSniperActionState() const
    {
        return ActionState;
    }

    /** @return 本领域本帧输入状态 */
    const FBBBSniperActionInputState &ReadSniperActionInputState() const
    {
        return ActionInputState;
    }

private:
    friend class FBBBSniperActionProcessor;
    friend class FBBBSniperParseProcessor;
    /** 本领域处理器维护的状态 */
    FBBBSniperActionState ActionState;

    /** 本领域解析后等待处理的输入 */
    FBBBSniperActionInputState ActionInputState;
};
