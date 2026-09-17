#pragma once

struct FBBBAimRuntimeState;
struct FBBBCharacterControlState;

/**
 * 根据角色意图生成瞄准状态
 */
class FBBBCharacterAimStateProcessor final
{
public:
    /**
     * 合并瞄准与开火意图
     * @param ControlData 角色意图数据
     * @param State      瞄准状态
     */
    void Update(
        const FBBBCharacterControlState &ControlData,
        FBBBAimRuntimeState &State) const;
};
