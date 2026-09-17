
#pragma once
struct FBBBCharacterEquipmentState;
struct FBBBDecisionRuntimeData;

class FBBBCharacterEquipmentStateArbitrator final
{
public:

    /**
     * 装备切换过程中驳回所有占用身体的待决请求
     * @param DecisionData	角色仲裁数据
     * @param EquipmentState	角色装备状态
     */
    void Update(
        FBBBDecisionRuntimeData &DecisionData,
        const FBBBCharacterEquipmentState &EquipmentState) const;
};
