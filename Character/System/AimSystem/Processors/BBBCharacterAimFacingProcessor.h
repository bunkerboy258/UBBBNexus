#pragma once

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
     * @param Movement          角色移动组件
     * @param State             瞄准状态
     */
    void Update(
        UCharacterMovementComponent &Movement,
        const FBBBAimRuntimeState &State) const;
};
