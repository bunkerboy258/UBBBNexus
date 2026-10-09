#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/ParseSystem/DomainData/States/BBBShotgunInputState.h"

/** 霰弹枪Parse领域状态的唯一持有者 */
struct FBBBShotgunParseDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBShotgunInputState &ReadShotgunInputState() const
    {
        return InputState;
    }

private:
    friend class FBBBShotgunParseProcessor;
    /** 本领域处理器维护的状态 */
    FBBBShotgunInputState InputState;
};
