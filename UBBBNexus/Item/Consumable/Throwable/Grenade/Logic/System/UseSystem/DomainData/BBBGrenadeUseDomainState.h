#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/UseSystem/DomainData/States/BBBGrenadeUseState.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/UseSystem/DomainData/States/BBBGrenadeUseInputState.h"
/** 使用状态的唯一持有者 */
struct FBBBGrenadeUseDomainState final
{
    /** @return 当次使用状态 */
    const FBBBGrenadeUseState &ReadGrenadeUseState() const
    {
        return UseState;
    }
    /** @return 当帧使用输入 */
    const FBBBGrenadeUseInputState &ReadGrenadeUseInputState() const
    {
        return UseInputState;
    }

  private:
    friend class FBBBGrenadeParseProcessor;
    friend class FBBBGrenadeUseProcessor;
    friend class FBBBGrenadeFinishProcessor;
    FBBBGrenadeUseState UseState;
    FBBBGrenadeUseInputState UseInputState;
};
