#pragma once
/** 一种掉落物输入的唯一排队槽位 */
template<typename TPacket>
struct TBBBWorldDropInputSlot final
{
    /** 本槽位是否含待处理输入 */
    bool bActive = false;
    /** 同帧累计的数据包 */
    TPacket Data;
};
