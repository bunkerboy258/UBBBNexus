#pragma once

/** 通用次行为请求 */
struct FBBBEquipmentSecondaryLocalControlPacket final
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
