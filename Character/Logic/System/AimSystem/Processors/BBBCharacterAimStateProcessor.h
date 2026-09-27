#pragma once

struct FBBBAimState;
struct FBBBCharacterControlState;

/**
 * 根据角色意图生成瞄准状态
 */
class FBBBCharacterAimStateProcessor final
{
public:
    /**
     * 读取通用瞄准意图
     * @param ControlData 角色意图数据
     * @param State      瞄准状态
     */
    void Update(
        const FBBBCharacterControlState &ControlData,
        FBBBAimState &State) const;

};
