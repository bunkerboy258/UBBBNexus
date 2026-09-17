#include "BBBWork/UBBBNexus/Equipment/Instance/System/EquipSystem/Fragment/BBBStandardEquipFragment.h"

#include "BBBWork/UBBBNexus/Character/Instance/ExternalAPI/BBBCharacterExternalAPI.h"
#include "Animation/AnimMontage.h"

bool FBBBStandardEquipFragment::SubmitMontage(FBBBCharacterExternalAPI &CharacterAPI, const int32 Sequence) const
{
    // 未配置切换动画时直接视为表现成功
    return !EquipMontage || CharacterAPI.SubmitEquipmentMontage(EquipMontage, 1.0f, Sequence);
}
