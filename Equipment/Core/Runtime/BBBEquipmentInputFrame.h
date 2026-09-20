#pragma once

#include "CoreMinimal.h"

/** 单类装备输入在当前帧中的固定槽位 */
template<typename TPacket>
struct TBBBEquipmentInputSlot
{
    bool bActive = false;

    TPacket Packet;

    void Submit(const TPacket &InPacket)
    {
        Packet = InPacket;
        bActive = true;
    }

    bool Consume(TPacket &OutPacket)
    {
        if (!bActive)
        {
            return false;
        }

        OutPacket = Packet;
        bActive = false;
        return true;
    }

    void Reset()
    {
        bActive = false;
    }
};

/** 由武器签名确定的固定输入帧 */
template<typename... TPackets>
struct TBBBEquipmentInputFrame : TBBBEquipmentInputSlot<TPackets>...
{
    template<typename TPacket>
    void Submit(const TPacket &Packet)
    {
        static_cast<TBBBEquipmentInputSlot<TPacket> &>(*this).Submit(Packet);
    }

    template<typename TPacket>
    bool Consume(TPacket &OutPacket)
    {
        return static_cast<TBBBEquipmentInputSlot<TPacket> &>(*this).Consume(OutPacket);
    }

    void Reset()
    {
        (TBBBEquipmentInputSlot<TPackets>::Reset(), ...);
    }
};
