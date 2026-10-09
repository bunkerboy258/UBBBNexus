#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/ParseSystem/DomainData/States/BBBMinigunInputState.h"

/** 转管机枪Parse领域状态的唯一持有者 */
struct FBBBMinigunParseDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBMinigunInputState &ReadMinigunInputState() const
    {
        return InputState;
    }

private:
    friend class FBBBMinigunParseProcessor;
    /** 本领域处理器维护的状态 */
    FBBBMinigunInputState InputState;
};
