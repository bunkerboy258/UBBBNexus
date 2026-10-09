#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/ParseSystem/DomainData/States/BBBSMGInputState.h"

/** 冲锋枪Parse领域状态的唯一持有者 */
struct FBBBSMGParseDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBSMGInputState &ReadSMGInputState() const
    {
        return InputState;
    }

private:
    friend class FBBBSMGParseProcessor;
    /** 本领域处理器维护的状态 */
    FBBBSMGInputState InputState;
};
