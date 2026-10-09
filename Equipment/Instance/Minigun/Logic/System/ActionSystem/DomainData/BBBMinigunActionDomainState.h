#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/ActionSystem/DomainData/States/BBBMinigunActionInputState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/ActionSystem/DomainData/States/BBBMinigunActionState.h"

/** 转管机枪Action领域状态的唯一持有者 */
struct FBBBMinigunActionDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBMinigunActionState &ReadMinigunActionState() const
    {
        return ActionState;
    }

    /** @return 本领域本帧输入状态 */
    const FBBBMinigunActionInputState &ReadMinigunActionInputState() const
    {
        return ActionInputState;
    }

private:
    friend class FBBBMinigunActionProcessor;
    friend class FBBBMinigunParseProcessor;
    /** 本领域处理器维护的状态 */
    FBBBMinigunActionState ActionState;

    /** 本领域解析后等待处理的输入 */
    FBBBMinigunActionInputState ActionInputState;
};
