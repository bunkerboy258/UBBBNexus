#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/DomainData/States/BBBRifleState.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Input/BBBEquipmentInputState.h"

struct FBBBRifleSignature;
template<typename TSignature>
struct TBBBEquipmentRuntime;

/** 步枪行为状态与固定输入状态的唯一持有者 */
struct FBBBRifleDomainState final
{
public:
    /** @return 步枪当前行为状态 */
    const FBBBRifleState &ReadRifleState() const
    {
        return State;
    }

    /** @return 步枪当前固定输入状态 */
    const FBBBEquipmentInputState &ReadEquipmentInputState() const
    {
        return InputState;
    }

private:
    friend struct FBBBRifleSignature;
    template<typename TSignature>
    friend struct TBBBEquipmentRuntime;

    /** 步枪当前行为状态 */
    FBBBRifleState State;

    /** 步枪固定输入状态 */
    FBBBEquipmentInputState InputState;
};
