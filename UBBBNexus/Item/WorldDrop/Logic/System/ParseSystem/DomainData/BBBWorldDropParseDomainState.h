#pragma once
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/ParseSystem/DomainData/States/BBBWorldDropInputState.h"
/** 固定输入槽位的唯一持有者 */
struct FBBBWorldDropParseDomainState final
{
public:
    /** @return 固定输入槽位的只读快照 */
    const FBBBWorldDropInputState &ReadWorldDropInputState() const
    {
        return InputState;
    }
private:
    friend class FBBBWorldDropParseSystem;
    /** 所有外部输入的唯一队列 */
    FBBBWorldDropInputState InputState;
};
