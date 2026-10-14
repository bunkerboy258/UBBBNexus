#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/DomainData/States/BBBGrenadeUseState.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/DomainData/States/BBBGrenadeUseInputState.h"

/** 手雷Use领域的唯一持有者 */
struct FBBBGrenadeUseDomainState final
{
public:
    /** @return UseState 的只读结果 */
    const FBBBGrenadeUseState &ReadGrenadeUseState() const
    {
        return UseState;
    }

    /** @return UseInputState 的只读结果 */
    const FBBBGrenadeUseInputState &ReadGrenadeUseInputState() const
    {
        return UseInputState;
    }
private:
    friend class FBBBGrenadeUseProcessor;
    friend class FBBBGrenadeLaunchProcessor;
    friend class FBBBGrenadeFinishProcessor;
    friend class FBBBGrenadeParseProcessor;
    FBBBGrenadeUseState UseState;
    FBBBGrenadeUseInputState UseInputState;
};
