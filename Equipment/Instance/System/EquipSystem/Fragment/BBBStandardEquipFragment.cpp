#include "BBBWork/UBBBNexus/Equipment/Instance/System/EquipSystem/Fragment/BBBStandardEquipFragment.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBMontagePacket.h"
#include "Animation/AnimMontage.h"

bool FBBBStandardEquipFragment::SubmitMontage(ABBBCharacter &CharacterAPI, const int32 Sequence) const
{
    // 未配置切换动画时直接视为表现成功
    if (!EquipMontage)
    {
        return true;
    }

    FBBBMontagePacket Packet;
    Packet.Montage = EquipMontage;
    Packet.PlayRate = 1.0f;
    Packet.Sequence = Sequence;
    return CharacterAPI.SubmitInput(Packet);
}
