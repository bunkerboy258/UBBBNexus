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

BBB_CHARACTER_INPUT_SLOT(FBBBAimStatePacket, AimState)
BBB_CHARACTER_INPUT_SLOT(FBBBRunStatePacket, RunState)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterMovementPacket, Movement)
BBB_CHARACTER_INPUT_SLOT(FBBBRunPacket, Run)
BBB_CHARACTER_INPUT_SLOT(FBBBCrouchPacket, Crouch)
BBB_CHARACTER_INPUT_SLOT(FBBBCharacterAimPacket, Aim)
BBB_CHARACTER_INPUT_SLOT(FBBBJumpPacket, Jump)
BBB_CHARACTER_INPUT_SLOT(FBBBFullBodyMontagePacket, FullBodyMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBUpperBodyMontagePacket, UpperBodyMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBFullBodyAdditivePreAimMontagePacket, FullBodyAdditivePreAimMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBUpperBodyAdditiveMontagePacket, UpperBodyAdditiveMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBAdditiveHitReactMontagePacket, AdditiveHitReactMontage)
BBB_CHARACTER_INPUT_SLOT(FBBBCameraPacket, Camera)

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
        using FPacket = typename TDecay<TPacket>::Type;

        if (!ensureMsgf(IsInGameThread() && Packet.IsValid() && !State.bProcessing,
            TEXT("角色输入被拒绝 请检查线程 数据与解析重入")))
        {
            return false;
        }

        TBBBCharacterInputSlot<FPacket> &Slot = TBBBCharacterInputSlotSelector<FPacket>::Get(State);
        Slot.Data = Forward<TPacket>(Packet);
        Slot.bActive = true;
        return true;
    }
}
