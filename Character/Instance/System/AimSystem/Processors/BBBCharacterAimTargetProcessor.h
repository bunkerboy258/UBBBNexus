#pragma once

class APawn;
struct FBBBAimRuntimeState;

/**
 * 根据控制器视线生成远距瞄准目标
 */
class FBBBCharacterAimTargetProcessor final
{
public:
    /**
     * 沿控制器视线投射瞄准目标
     * @param Pawn              所属角色
     * @param AimTargetDistance 目标投射距离
     * @param State             瞄准状态
     */
    void Update(
        APawn &Pawn,
        float AimTargetDistance,
        FBBBAimRuntimeState &State) const;
};
