#pragma once

/** 单种角色输入的固定驻留槽位 */
template<typename TPacket>
struct TBBBCharacterInputSlot final
{
    /** 本帧或上次解析后是否收到输入 */
    bool bActive = false;

    /** 同类型输入最后一次提交的数据 */
    TPacket Data;
};
