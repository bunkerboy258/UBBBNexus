#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterControlInput.h"
#include "BBBCharacterControlState.generated.h"

/** 已裁决的角色控制黑板 */
USTRUCT()
struct FBBBCharacterControlState
{
    GENERATED_BODY()

    /** @return 世界空间移动输入 */
    const FVector &GetMoveInput() const
    {
        return Value.MoveWorld;
    }
    /** @return 是否存在移动输入 */
    bool HasMoveInput() const
    {
        return !Value.MoveWorld.IsNearlyZero();
    }
    /** @return 已批准开火状态 */
    bool WantsFire() const
    {
        return Value.bFire;
    }
    /** @return 已批准瞄准状态 */
    bool WantsAim() const
    {
        return Value.bAim;
    }
    /** @return 已批准行走状态 */
    bool WantsWalk() const
    {
        return Value.bWalk;
    }
    /** @return 已批准冲刺状态 */
    bool WantsSprint() const
    {
        return Value.bSprint;
    }
    /** @return 已批准蹲伏状态 */
    bool WantsCrouch() const
    {
        return Value.bCrouch;
    }
    /** @return 已批准跳跃事件 */
    bool WantsJump() const
    {
        return Value.bJump;
    }
    /** @return 世界空间朝向 */
    const FRotator &GetFacingWorld() const
    {
        return Value.FacingWorld;
    }
    /** @return 世界空间瞄准目标 */
    const FVector &GetAimTargetWorld() const
    {
        return Value.AimTargetWorld;
    }

private:
    friend class FBBBArbitrationPipeline;

    UPROPERTY()
    FBBBCharacterControlInput Value;
};
