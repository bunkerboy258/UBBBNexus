#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/ActionSystem/DomainData/States/BBBPistolActionInputState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/ActionSystem/DomainData/States/BBBPistolActionState.h"

/** 手枪Action领域状态的唯一持有者 */
struct FBBBPistolActionDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBPistolActionState &ReadPistolActionState() const
    {
        return ActionState;
    }

    /** @return 本领域本帧输入状态 */
    const FBBBPistolActionInputState &ReadPistolActionInputState() const
    {
        return ActionInputState;
    }

private:
    friend class FBBBPistolActionProcessor;
    friend class FBBBPistolParseProcessor;
    /** 本领域处理器维护的状态 */
    FBBBPistolActionState ActionState;

    /** 本领域解析后等待处理的输入 */
    FBBBPistolActionInputState ActionInputState;
};
