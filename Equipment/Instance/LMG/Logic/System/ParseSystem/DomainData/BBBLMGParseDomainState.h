#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/ParseSystem/DomainData/States/BBBLMGInputState.h"

/** 轻机枪Parse领域状态的唯一持有者 */
struct FBBBLMGParseDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBLMGInputState &ReadLMGInputState() const
    {
        return InputState;
    }

private:
    friend class FBBBLMGParseProcessor;
    /** 本领域处理器维护的状态 */
    FBBBLMGInputState InputState;
};
