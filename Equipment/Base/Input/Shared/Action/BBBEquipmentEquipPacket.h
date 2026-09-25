#pragma once

/** 建立装备持有关系后的表现请求 */
struct FBBBEquipmentEquipPacket final
{
    /** @return 包内容是否合法 */
    bool IsValid() const
    {
        return true;
    }
};
