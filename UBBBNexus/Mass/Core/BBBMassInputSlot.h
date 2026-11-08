#pragma once

/** 每目标每种输入的覆盖槽 */
template<typename TPacket>
struct TBBBMassInputSlot
{
    /** 是否等待固定阶段消费 */
    bool bActive = false;

    /** 最近一次提交的输入 */
    TPacket Packet;
};
