
#pragma once
struct FBBBCharacterItemCommands;
struct FBBBDecisionRuntimeData;

class FBBBItemActionExecutor final
{
public:

    void Update(
        FBBBDecisionRuntimeData &DecisionData,
        FBBBCharacterItemCommands &ItemCommands) const;
};
