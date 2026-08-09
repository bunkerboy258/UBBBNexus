#pragma once

class APawn;
class UCharacterMovementComponent;
struct FBBBAimRuntimeState;

/**
 * 根据瞄准状态切换角色朝向模式
 */
class FBBBCharacterAimFacingProcessor final
{
public:
    /**
     * 瞄准时追随控制器朝向，否则恢复移动朝向
     * @param Pawn              所属角色
     * @param Movement          角色移动组件
     * @param DeltaSeconds      当前帧时间
     * @param FacingInterpSpeed 瞄准转向速度
     * @param State             瞄准状态
     */
    void Update(
        APawn &Pawn,
        UCharacterMovementComponent &Movement,
        float DeltaSeconds,
        float FacingInterpSpeed,
        const FBBBAimRuntimeState &State) const;
};
