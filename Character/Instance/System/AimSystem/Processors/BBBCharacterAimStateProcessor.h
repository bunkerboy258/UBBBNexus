#pragma once

struct FBBBAimRuntimeState;
struct FBBBIntentRuntimeData;

/**
 * 根据角色意图生成瞄准状态
 */
class FBBBCharacterAimStateProcessor final
{
public:
    /**
     * 合并瞄准与开火意图
     * @param IntentData 角色意图数据
     * @param State      瞄准状态
     */
    void Update(
        const FBBBIntentRuntimeData &IntentData,
        FBBBAimRuntimeState &State) const;
};
