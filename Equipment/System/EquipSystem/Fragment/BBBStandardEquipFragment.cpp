#include "BBBWork/UBBBNexus/Equipment/System/EquipSystem/Fragment/BBBStandardEquipFragment.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "Animation/AnimMontage.h"

bool FBBBStandardEquipFragment::SubmitMontage(FBBBCharacterExternalAPI &CharacterAPI, const int32 Sequence) const
{
    return !EquipMontage || CharacterAPI.SubmitEquipmentMontage(EquipMontage, 1.0f, Sequence);
}
