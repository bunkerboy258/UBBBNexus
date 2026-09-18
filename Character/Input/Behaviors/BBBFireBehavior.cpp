#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBFireBehavior.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"

bool FBBBFireBehavior::CanStart(const FInput &Input, const FBBBCharacterRuntimeData &Data)
{
    return true;
}

void FBBBFireBehavior::Start(const FInput &Input, FBBBCharacterRuntimeData &Data)
{
    Data.Operation.bFire = true;
    Data.Operation.ActiveBehaviorGroups |= BBBBehaviorGroup::Fire;
    Data.Operation.Control.bFire = true;
    Data.Equipment.Commands.SubmitFire();
}
