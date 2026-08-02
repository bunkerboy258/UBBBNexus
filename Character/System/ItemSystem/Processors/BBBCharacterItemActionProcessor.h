
#pragma once
struct FBBBCharacterEquipmentState;
struct FBBBCharacterItemCommands;

class FBBBCharacterItemActionProcessor final
{
public:

    void Update(
        FBBBCharacterItemCommands &ItemCommands,
        const FBBBCharacterEquipmentState &EquipmentState) const;
};
