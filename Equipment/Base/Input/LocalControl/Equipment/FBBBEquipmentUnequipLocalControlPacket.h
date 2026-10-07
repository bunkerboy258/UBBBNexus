#pragma once

/** 解除本机装备持有请求 */
struct FBBBEquipmentUnequipLocalControlPacket final
{
    /** @return 请求数据有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 请求可进入具体装备解析 */
    bool CanApply() const
    {
        return true;
    }

    /** @return 无 具体装备解释公共请求 */
    void Apply() const
    {
    }
};
