#pragma once
#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBBehaviorPolicy.h"
#include <concepts>

struct FBBBCharacterRuntimeData;

/** 行为数据与规则必须在编译期形成完整契约 */
template <typename TBehavior>
concept CBBBBehavior = requires(const typename TBehavior::FInput &Input,
    const FBBBCharacterRuntimeData &Read, FBBBCharacterRuntimeData &Write)
{
    { TBehavior::Policy } -> std::convertible_to<FBBBBehaviorPolicy>;
    { TBehavior::CanStart(Input, Read) } -> std::convertible_to<bool>;
    TBehavior::Start(Input, Write);
};
