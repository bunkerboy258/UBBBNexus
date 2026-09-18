#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterControlInput.h"

bool FBBBCharacterControlRule::AllowsSprint(const bool bAiming, const bool bFiring)
{
    return !bAiming && !bFiring;
}
