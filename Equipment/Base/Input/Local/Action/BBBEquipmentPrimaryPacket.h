#pragma once

/** 装备本机主行为请求 */
struct FBBBEquipmentPrimaryPacket final
{
    /** @return 包内容是否合法 */
    bool IsValid() const
    {
        return true;
    }
};
