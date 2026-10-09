#pragma once

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterStimulusFragment.h"

struct FBBBMonsterPerceptionInputFragment;

/** 预留声音线索输入 调用方负责声源距离和主机投递 */
struct FBBBMonsterSoundLocalControlPacket final
{
    using FInputFragment = FBBBMonsterPerceptionInputFragment;

    /** 声音发生位置 不持有玩家实时位置 */
    FVector Position = FVector::ZeroVector;

    /** 主机世界时间 */
    float Time = 0.0f;

    /** 声音线索保留秒数 */
    float Duration = 3.0f;

    /** @return 是否为有限的有效声音线索 */
    bool IsValid() const
    {
        return !Position.ContainsNaN() && FMath::IsFinite(Time) && Time >= 0.0f
            && FMath::IsFinite(Duration) && Duration > 0.0f && Duration <= 6.0f;
    }

    /**
     * @param State	当前声音线索
     * @return 新线索不能被旧输入覆盖
     */
    bool CanApply(const FBBBMonsterStimulusFragment& State) const
    {
        return Time > State.Time;
    }

    /**
     * @param State	当前声音线索
     * @return 无返回值
     */
    void Apply(FBBBMonsterStimulusFragment& State) const
    {
        State.Position = Position;
        State.Time = Time;
        State.EndsAt = Time + Duration;
    }
};
