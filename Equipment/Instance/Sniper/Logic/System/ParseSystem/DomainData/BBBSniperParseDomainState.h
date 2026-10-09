#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/ParseSystem/DomainData/States/BBBSniperInputState.h"

/** 狙击枪Parse领域状态的唯一持有者 */
struct FBBBSniperParseDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBSniperInputState &ReadSniperInputState() const
    {
        return InputState;
    }

private:
    friend class FBBBSniperParseProcessor;
    /** 本领域处理器维护的状态 */
    FBBBSniperInputState InputState;
};
