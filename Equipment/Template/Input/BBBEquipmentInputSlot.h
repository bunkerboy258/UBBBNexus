#pragma once

/** 单种装备输入在当前解析帧中的固定槽位 */
template<typename TPacket>
struct TBBBEquipmentInputSlot final
{
    /** 本帧是否收到该输入 */
    bool bActive = false;

    /** 本次输入是否只能执行镜像还原 */
    bool bIsMirror = false;

    /** 同类输入最后一次提交的数据 */
    TPacket Packet;
};
