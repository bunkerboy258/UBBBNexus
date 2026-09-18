#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBFireBehavior.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"

bool FBBBFireBehavior::AllowsFire(const bool bReloading, const bool bSwitching,
    const bool bStartingReload)
{
    return !bReloading && !bSwitching && !bStartingReload;
}

bool FBBBFireBehavior::CanStart(const FInput &Input, const FBBBCharacterRuntimeData &Data)
{
    return AllowsFire(Data.Operation.ReloadSequence > 0,
        Data.Operation.SelectedEquipment != nullptr, Data.Operation.bReload);
}

void FBBBFireBehavior::Start(const FInput &Input, FBBBCharacterRuntimeData &Data)
{
    Data.Operation.bFire = true;
    Data.Operation.Control.bFire = true;
    Data.Equipment.Commands.SubmitFire();
}
