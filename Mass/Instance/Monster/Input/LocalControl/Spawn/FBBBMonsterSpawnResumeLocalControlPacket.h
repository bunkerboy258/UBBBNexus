#pragma once
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterSpawnFragment.h"

struct FBBBMonsterSpawnInputFragment;

/** 继续指定入口的当前出生请求 */
struct FBBBMonsterSpawnResumeLocalControlPacket final
{
    using FInputFragment = FBBBMonsterSpawnInputFragment;
    /** @return 控制请求不携带额外数值 */
    bool IsValid() const
    {
        return true;
    }
    /**
     * @param State	当前请求
     * @return 是否允许继续
     */
    bool CanApply(const FBBBMonsterSpawnFragment& State) const
    {
        return State.bActive;
    }
    /**
     * @param State	当前请求
     * @return 无返回值
     */
    void Apply(FBBBMonsterSpawnFragment& State) const
    {
        State.bPaused = false;
        State.Reason = TEXT("已继续");
    }
};
