#pragma once
/** 消耗品Show请求的编译期契约 */
template<typename TDerived = void>
struct FBBBConsumableShowLocalControlPacket
{
    bool IsValid() const;
    template<typename TState> bool CanApply(bool bIsMirror, const TState &State) const;
    template<typename TInput, typename TState> void Apply(TInput &Input, TState &State) const;
};
