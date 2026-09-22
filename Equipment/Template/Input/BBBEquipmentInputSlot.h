#pragma once

/** 单种装备输入在当前解析帧中的固定槽位 */
template<typename TPacket>
struct TBBBEquipmentInputSlot final
{
    /** 本帧是否收到该输入 */
    bool bActive = false;

    /** 同类输入最后一次提交的数据 */
    TPacket Packet;

    /**
     * 覆盖当前帧输入
     * @param InPacket 本次输入数据
     * @return 无
     */
    void Submit(const TPacket &InPacket)
    {
        Packet = InPacket;
        bActive = true;
    }

    /** 清除当前帧输入 */
    void Reset()
    {
        bActive = false;
    }
};
