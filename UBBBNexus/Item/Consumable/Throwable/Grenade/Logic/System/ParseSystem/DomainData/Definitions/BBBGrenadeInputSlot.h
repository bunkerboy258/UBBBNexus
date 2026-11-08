#pragma once
/** 每种请求的固定输入槽 */
template <typename TPacket> struct TBBBGrenadeInputSlot final
{
    /** 是否待消费 */
    bool bActive = false;
    /** 输入值 */
    TPacket Packet;
};
