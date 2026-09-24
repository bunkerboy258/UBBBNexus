#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ActionSystem/DomainData/States/BBBRifleActionState.h"

/** 步枪Action领域状态的唯一持有者 */
struct FBBBRifleActionDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBRifleActionState &ReadRifleActionState() const
    {
        return ActionState;
    }

private:
    friend class FBBBRifleActionProcessor;
    /** 本领域处理器维护的状态 */
    FBBBRifleActionState ActionState;
};
