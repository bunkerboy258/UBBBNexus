
#pragma once
struct FBBBCharacterEquipmentState;
struct FBBBCharacterItemActionResults;
struct FBBBCharacterItemCommands;

class FBBBCharacterItemActionProcessor final
{
public:

    void Update(
        FBBBCharacterItemCommands &ItemCommands,
        const FBBBCharacterEquipmentState &EquipmentState,
        FBBBCharacterItemActionResults &ActionResults) const;
};
