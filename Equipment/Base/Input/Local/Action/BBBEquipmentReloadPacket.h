#pragma once

/** 装备本机换弹请求 */
struct FBBBEquipmentReloadPacket final
{
    /** @return 包内容是否合法 */
    bool IsValid() const
    {
        return true;
    }
};
