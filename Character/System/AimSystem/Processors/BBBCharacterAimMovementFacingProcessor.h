#pragma once

class APawn;
class UCharacterMovementComponent;
struct FBBBAimRuntimeState;

/**
 * 恢复普通移动朝向或驱动移动瞄准朝向
 */
class FBBBCharacterAimMovementFacingProcessor final
{
public:
    /**
     * 根据瞄准状态切换移动朝向模式
     * @param Pawn              所属角色
     * @param Movement          角色移动组件
     * @param DeltaSeconds      当前帧时间
     * @param FacingInterpSpeed 移动瞄准转向速度
     * @param State             瞄准状态
     */
    void Update(
        APawn &Pawn,
        UCharacterMovementComponent &Movement,
        float DeltaSeconds,
        float FacingInterpSpeed,
        FBBBAimRuntimeState &State) const;
};
