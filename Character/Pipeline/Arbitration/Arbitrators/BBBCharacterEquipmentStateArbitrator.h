
#pragma once
struct FBBBCharacterEquipmentState;
struct FBBBDecisionRuntimeData;

class FBBBCharacterEquipmentStateArbitrator final
{
public:

    void Update(
        FBBBDecisionRuntimeData &DecisionData,
        const FBBBCharacterEquipmentState &EquipmentState) const;
};
