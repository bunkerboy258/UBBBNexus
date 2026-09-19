#pragma once

#include "CoreMinimal.h"

struct FBBBCharacterPacketContext;

/**
 * 移动基底快照包 每帧覆盖控制基座的移动与步态意图
 * 本帧未提交时黑板保留上帧值
 */
struct FBBBCharacterMovementPacket final
{

    FVector MoveWorld = FVector::ZeroVector;

    FRotator FacingWorld = FRotator::ZeroRotator;

    bool bWalk = false;

    bool bSprint = false;

    bool bCrouch = false;

    /** @return 包内容是否合法 */
    bool IsValid() const;

    /**
     * 检查本帧是否允许执行
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanApply(const FBBBCharacterPacketContext &Context) const;

    /**
     * 覆盖控制基座的移动与步态意图
     * @param Context	黑板上下文
     */
    void Apply(FBBBCharacterPacketContext &Context) const;
};
