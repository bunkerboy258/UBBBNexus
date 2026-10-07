#pragma once
/** 近战单种输入的固定槽位 */
template<typename TPacket>
struct TBBBMeleeInputSlot final
{
    /** 当前槽位有待消费的数据 */
    bool bActive = false;
    /** 同类通知实例标识在一帧内累计 */
    TPacket Packet;
private:
    friend struct FBBBMeleeInputState;
    TBBBMeleeInputSlot() = default;
};
