#pragma once
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterSpawnFragment.h"

struct FBBBMonsterSpawnInputFragment;

/** 停止指定入口的当前出生请求 */
struct FBBBMonsterSpawnStopLocalControlPacket final
{
    using FInputFragment = FBBBMonsterSpawnInputFragment;
    /** @return 控制请求不携带额外数值 */
    bool IsValid() const
    {
        return true;
    }
    /**
     * @param State	当前请求
     * @return 是否允许停止
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
        State.bActive = false;
        State.Reason = TEXT("已停止");
    }
};
