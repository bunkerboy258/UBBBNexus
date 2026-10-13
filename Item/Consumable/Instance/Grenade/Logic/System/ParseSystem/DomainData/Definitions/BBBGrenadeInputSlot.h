#pragma once
/** 一种输入的固定槽位 */
template<typename TPacket>
struct TBBBGrenadeInputSlot final
{
    /** 本帧是否有输入 */
    bool bActive = false;
    /** 本帧已合并的输入 */
    TPacket Data;
};
