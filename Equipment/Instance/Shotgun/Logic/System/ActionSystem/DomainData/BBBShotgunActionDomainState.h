#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/ActionSystem/DomainData/States/BBBShotgunActionInputState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/ActionSystem/DomainData/States/BBBShotgunActionState.h"

/** 霰弹枪Action领域状态的唯一持有者 */
struct FBBBShotgunActionDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBShotgunActionState &ReadShotgunActionState() const
    {
        return ActionState;
    }

    /** @return 本领域本帧输入状态 */
    const FBBBShotgunActionInputState &ReadShotgunActionInputState() const
    {
        return ActionInputState;
    }

private:
    friend class FBBBShotgunActionProcessor;
    friend class FBBBShotgunParseProcessor;
    /** 本领域处理器维护的状态 */
    FBBBShotgunActionState ActionState;

    /** 本领域解析后等待处理的输入 */
    FBBBShotgunActionInputState ActionInputState;
};
