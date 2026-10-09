#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/ParseSystem/DomainData/States/BBBRevolverInputState.h"

/** 左轮Parse领域状态的唯一持有者 */
struct FBBBRevolverParseDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBRevolverInputState &ReadRevolverInputState() const
    {
        return InputState;
    }

private:
    friend class FBBBRevolverParseProcessor;
    /** 本领域处理器维护的状态 */
    FBBBRevolverInputState InputState;
};
