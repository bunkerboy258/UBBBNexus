#pragma once
#include "CoreMinimal.h"

class APawn;

/** 枪弹命中后提交的玩家来源 */
struct FBBBMaodieTestProjectileHitLocalControlPacket final
{
    /** 当前槽位累计的射手 */
    TArray<TWeakObjectPtr<APawn>> Shooters;

    /** @return 是否携带命中来源 */
    bool IsValid() const
    {
        return !Shooters.IsEmpty();
    }

    /** @param bCausal 当前是否维护行为 @return 是否接收本次贡献 */
    bool CanApply(bool bCausal) const
    {
        return bCausal;
    }

    /** @param PendingShooters 待处理的射手队列 @return 无 */
    void Apply(TArray<TWeakObjectPtr<APawn>> &PendingShooters) const
    {
        PendingShooters.Append(Shooters);
    }
};
