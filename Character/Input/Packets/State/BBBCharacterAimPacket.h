#pragma once

#include "CoreMinimal.h"

struct FBBBCharacterInputContext;

/**
 * 瞄准基底快照包 每帧覆盖控制基座的瞄准意图
 * 本帧未提交时黑板保留上帧值
 */
struct FBBBCharacterAimPacket final
{

    FVector AimTargetWorld = FVector::ZeroVector;

    bool bAim = false;

    /** @return 包内容是否合法 */
    bool IsValid() const;

    /**
     * 检查本帧是否允许执行
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanApply(const FBBBCharacterInputContext &Context) const;

    /**
     * 覆盖控制基座的瞄准意图
     * @param Context	黑板上下文
     */
    void Apply(FBBBCharacterInputContext &Context) const;
};
