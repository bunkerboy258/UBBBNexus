#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ActionSystem/DomainData/States/BBBRifleActionInputState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ActionSystem/DomainData/States/BBBRifleActionState.h"

/** 步枪Action领域状态的唯一持有者 */
struct FBBBRifleActionDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBRifleActionState &ReadRifleActionState() const
    {
        return ActionState;
    }

    /** @return 本领域本帧输入状态 */
    const FBBBRifleActionInputState &ReadRifleActionInputState() const
    {
        return ActionInputState;
    }

private:
    friend class FBBBRifleActionProcessor;
    friend class FBBBRifleParseProcessor;
    /** 本领域处理器维护的状态 */
    FBBBRifleActionState ActionState;

    /** 本领域解析后等待处理的输入 */
    FBBBRifleActionInputState ActionInputState;
};
