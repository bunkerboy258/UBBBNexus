#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Arbitration/Rules/BBBCharacterConflictRules.h"

bool FBBBCharacterConflictRules::AllowsFire(bool bReloading, bool bSwitching, bool bStartingReload)
{
    return !bReloading && !bSwitching && !bStartingReload;
}

bool FBBBCharacterConflictRules::AllowsSprint(bool bAiming)
{
    return !bAiming;
}
