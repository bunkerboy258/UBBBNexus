#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/ParseSystem/DomainData/States/BBBPistolInputState.h"

/** 手枪Parse领域状态的唯一持有者 */
struct FBBBPistolParseDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBPistolInputState &ReadPistolInputState() const
    {
        return InputState;
    }

private:
    friend class FBBBPistolParseProcessor;
    /** 本领域处理器维护的状态 */
    FBBBPistolInputState InputState;
};
