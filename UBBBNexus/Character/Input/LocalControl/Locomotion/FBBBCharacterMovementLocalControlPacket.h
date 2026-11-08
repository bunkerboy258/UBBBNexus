#pragma once

#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

#include "CoreMinimal.h"

struct FBBBCharacterInputContext;

/**
 * 移动基底快照包 每帧覆盖控制基座的移动与朝向意图
 * 本帧未提交时黑板保留上帧值
 */
struct FBBBCharacterMovementLocalControlPacket final
{

    FVector MoveWorld = FVector::ZeroVector;

    FRotator FacingWorld = FRotator::ZeroRotator;

    /** @return 包内容是否合法 */
    bool IsValid() const
    {
        return !MoveWorld.ContainsNaN() && !FacingWorld.ContainsNaN();
    }

    /**
     * 检查本帧是否允许执行
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return true;
    }

    /**
     * 覆盖控制基座的移动与朝向意图
     * @param Context	黑板上下文
     */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.Control.MoveWorld = MoveWorld.GetClampedToMaxSize(1.0f);
        Context.Control.FacingWorld = FacingWorld;
    }
};
