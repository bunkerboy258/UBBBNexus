#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/ParseSystem/DomainData/States/BBBGrenadeInputState.h"

/** 手雷Parse领域的唯一持有者 */
struct FBBBGrenadeParseDomainState final
{
public:
    /** @return InputState 的只读结果 */
    const FBBBGrenadeInputState &ReadGrenadeInputState() const
    {
        return InputState;
    }
private:
    friend class FBBBGrenadeParseSystem;
    friend class FBBBGrenadeParseProcessor;
    friend class FBBBGrenadeInitializer;
    FBBBGrenadeInputState InputState;
};
