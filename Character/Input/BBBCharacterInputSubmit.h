#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/States/BBBCharacterInputState.h"

/** 角色输入包到固定槽位的编译期映射 */
template<typename TPacket>
struct TBBBCharacterInputSlotSelector;

#define BBB_CHARACTER_INPUT_SLOT(PacketType, MemberName) \
    template<> \
    struct TBBBCharacterInputSlotSelector<PacketType> final \
    { \
        static TBBBCharacterInputSlot<PacketType> &Get(FBBBCharacterInputState &State) \
        { \
            return State.MemberName; \
        } \
    };

BBB_CHARACTER_INPUT_SLOT(FBBBAimStateAuthorityFactPacket, AimState)
BBB_CHARACTER_INPUT_SLOT(FBBBRunStateAuthorityFactPacket, RunState)
BBB_CHARACTER_INPUT_SLOT(FBBBEquipmentSelectionLocalControlPacket, EquipmentSelectionState)
BBB_CHARACTER_INPUT_SLOT(FBBBEquipmentSelectionAuthorityFactPacket, AuthorityEquipmentSelectionState)
BBB_CHARACTER_INPUT_SLOT(FBBBEquipmentSlotLocalControlPacket, EquipmentSlot)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterMovementLocalControlPacket, Movement)
BBB_CHARACTER_INPUT_SLOT(FBBBRunLocalControlPacket, Run)
BBB_CHARACTER_INPUT_SLOT(FBBBCrouchLocalControlPacket, Crouch)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterAimLocalControlPacket, Aim)
BBB_CHARACTER_INPUT_SLOT(FBBBJumpLocalControlPacket, Jump)
BBB_CHARACTER_INPUT_SLOT(FBBBFullBodyMontageLocalControlPacket, FullBodyMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBFullBodyMontageAuthorityFactPacket, AuthorityFullBodyMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBUpperBodyMontageLocalControlPacket, UpperBodyMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBUpperBodyMontageAuthorityFactPacket, AuthorityUpperBodyMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBFullBodyAdditivePreAimMontageLocalControlPacket, FullBodyAdditivePreAimMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBFullBodyAdditivePreAimMontageAuthorityFactPacket, AuthorityFullBodyAdditivePreAimMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBUpperBodyAdditiveMontageLocalControlPacket, UpperBodyAdditiveMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBUpperBodyAdditiveMontageAuthorityFactPacket, AuthorityUpperBodyAdditiveMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBAdditiveHitReactMontageLocalControlPacket, AdditiveHitReactMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBAdditiveHitReactMontageAuthorityFactPacket, AuthorityAdditiveHitReactMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBCameraLocalControlPacket, Camera)
BBB_CHARACTER_INPUT_SLOT(FBBBAimImpulseLocalControlPacket, AimImpulse)
BBB_CHARACTER_INPUT_SLOT(FBBBAimImpulseAuthorityFactPacket, AuthorityAimImpulse)

#undef BBB_CHARACTER_INPUT_SLOT

/** 角色固定输入槽位的唯一提交闸口 */
namespace BBBCharacterInput
{
    /**
     * 根据包类型把它放进对应固定槽位
     * @param State 角色固定输入状态
     * @param Packet 输入包
     * @return 是否接受输入
     */
    template<typename TPacket>
    bool Submit(FBBBCharacterInputState &State, TPacket &&Packet)
    {
        if (!ensureMsgf(Packet.IsValid(), TEXT("角色输入包数据无效")))
        {
            return false;
        }

        using FPacket = typename TDecay<TPacket>::Type;

        TBBBCharacterInputSlot<FPacket> &Slot = TBBBCharacterInputSlotSelector<FPacket>::Get(State);
        Slot.Data = Forward<TPacket>(Packet);
        Slot.bActive = true;
        return true;
    }
}
