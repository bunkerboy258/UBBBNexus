#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ParseSystem/DomainData/States/BBBRifleInputState.h"

/** 步枪Parse领域状态的唯一持有者 */
struct FBBBRifleParseDomainState final
{
    /** @return 本领域持久状态 */
    const FBBBRifleInputState &ReadRifleInputState() const
    {
        return InputState;
    }

private:
    friend class FBBBRifleParseProcessor;
    friend class FBBBRifleActionProcessor;
    friend class FBBBRifleNetworkProcessor;
    /** 本领域处理器维护的状态 */
    FBBBRifleInputState InputState;
};
