#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/System/ParseSystem/DomainData/States/BBBHealthPackInputState.h"
/** 输入领域唯一持有者 */
struct FBBBHealthPackParseDomainState final
{
    /** @return 固定输入状态 */
    const FBBBHealthPackInputState &ReadHealthPackInputState() const
    {
        return InputState;
    }

  private:
    friend class FBBBHealthPackParseSystem;
    friend class FBBBHealthPackParseProcessor;
    FBBBHealthPackInputState InputState;
};
