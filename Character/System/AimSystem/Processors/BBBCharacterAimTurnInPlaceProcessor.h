#pragma once

class APawn;
class UCharacterMovementComponent;
struct FBBBAimConfig;
struct FBBBAimRuntimeState;

/**
 * 驱动静止瞄准时的区间原地转身
 */
class FBBBCharacterAimTurnInPlaceProcessor final
{
public:
    /**
     * 根据控制器与角色的水平夹角启停原地转身
     * @param Pawn         所属角色
     * @param Movement     角色移动组件
     * @param DeltaSeconds 当前帧时间
     * @param Config       瞄准配置
     * @param State        瞄准状态
     */
    void Update(
        APawn &Pawn,
        UCharacterMovementComponent &Movement,
        float DeltaSeconds,
        const FBBBAimConfig &Config,
        FBBBAimRuntimeState &State) const;
};
