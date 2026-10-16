#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/ParseSystem/DomainData/States/BBBGrenadeInputState.h"
/** 输入领域唯一持有者 */
struct FBBBGrenadeParseDomainState final
{
    /** @return 固定输入状态 */
    const FBBBGrenadeInputState &ReadGrenadeInputState() const
    {
        return InputState;
    }

  private:
    friend class FBBBGrenadeParseSystem;
    friend class FBBBGrenadeParseProcessor;
    FBBBGrenadeInputState InputState;
};
