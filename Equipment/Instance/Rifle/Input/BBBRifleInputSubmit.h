#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ParseSystem/DomainData/States/BBBRifleInputState.h"

/** 步枪输入包到固定槽位的编译期映射 */
template<typename TPacket>
struct TBBBRifleInputSlotSelector;

#define BBB_RIFLE_INPUT_SLOT(PacketType, MemberName) \
    template<> \
    struct TBBBRifleInputSlotSelector<PacketType> final \
    { \
        static TBBBRifleInputSlot<PacketType> &Get(FBBBRifleInputState &State) \
        { \
            return State.MemberName; \
        } \
    };

BBB_RIFLE_INPUT_SLOT(FBBBEquipmentEquipLocalControlPacket, Equip)
BBB_RIFLE_INPUT_SLOT(FBBBEquipmentEquipAuthorityFactPacket, AuthorityEquip)
BBB_RIFLE_INPUT_SLOT(FBBBEquipmentPrimaryLocalControlPacket, Primary)
BBB_RIFLE_INPUT_SLOT(FBBBEquipmentReloadLocalControlPacket, Reload)
BBB_RIFLE_INPUT_SLOT(FBBBRifleDetachMagazineLocalControlPacket, DetachMagazine)
BBB_RIFLE_INPUT_SLOT(FBBBRifleLoadMagazineLocalControlPacket, LoadMagazine)
BBB_RIFLE_INPUT_SLOT(FBBBRifleInterruptReloadLocalControlPacket, InterruptReload)
BBB_RIFLE_INPUT_SLOT(FBBBRifleActionStateAuthorityFactPacket, AuthorityActionState)

#undef BBB_RIFLE_INPUT_SLOT

/** 步枪固定输入槽位的唯一编译期提交入口 */
namespace BBBRifleInput
{
    /**
     * 按包的静态类型写入对应固定槽位
     * @param State	步枪固定输入状态
     * @param Packet	待提交输入包
     * @return 无
     */
    template<typename TPacket>
    void Submit(FBBBRifleInputState &State, const TPacket &Packet)
    {
        TBBBRifleInputSlot<TPacket> &Slot = TBBBRifleInputSlotSelector<TPacket>::Get(State);
        Slot.Packet = Packet;
        Slot.bActive = true;
    }
}
