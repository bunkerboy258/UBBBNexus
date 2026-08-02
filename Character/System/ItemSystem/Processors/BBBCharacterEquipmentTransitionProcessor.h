
#pragma once
struct FBBBCharacterEquipmentState;

class FBBBCharacterEquipmentTransitionProcessor final
{
public:

    void Update(float WorldTimeSeconds, FBBBCharacterEquipmentState &Equipment) const;
};
