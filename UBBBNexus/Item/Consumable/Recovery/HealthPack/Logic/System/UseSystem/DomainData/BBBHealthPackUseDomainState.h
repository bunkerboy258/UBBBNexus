#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/System/UseSystem/DomainData/States/BBBHealthPackUseState.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/System/UseSystem/DomainData/States/BBBHealthPackUseInputState.h"
/** 使用状态的唯一持有者 */
struct FBBBHealthPackUseDomainState final
{
    /** @return 当次使用状态 */
    const FBBBHealthPackUseState &ReadHealthPackUseState() const
    {
        return UseState;
    }
    /** @return 当帧使用输入 */
    const FBBBHealthPackUseInputState &ReadHealthPackUseInputState() const
    {
        return UseInputState;
    }

  private:
    friend class FBBBHealthPackParseProcessor;
    friend class FBBBHealthPackUseProcessor;
    friend class FBBBHealthPackFinishProcessor;
    FBBBHealthPackUseState UseState;
    FBBBHealthPackUseInputState UseInputState;
};
