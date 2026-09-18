#include "BBBWork/UBBBNexus/Equipment/Instance/System/EquipSystem/Fragment/BBBStandardEquipFragment.h"

#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterInput.h"
#include "Animation/AnimMontage.h"

bool FBBBStandardEquipFragment::SubmitMontage(FBBBCharacterInput &CharacterAPI, const int32 Sequence) const
{
    // 未配置切换动画时直接视为表现成功
    if (!EquipMontage)
    {
        return true;
    }

    FBBBCharacterDiscreteInput Input;
    Input.Montage.Montage = EquipMontage;
    Input.Montage.PlayRate = 1.0f;
    Input.Montage.Sequence = Sequence;
    return CharacterAPI.Submit(Input);
}
