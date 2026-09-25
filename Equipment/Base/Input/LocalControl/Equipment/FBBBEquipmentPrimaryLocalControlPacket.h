#pragma once

/** 装备本机主行为请求 */
struct FBBBEquipmentPrimaryLocalControlPacket final
{
    /** @return 包内容是否合法 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 是否允许提交主行为请求 */
    bool CanApply() const
    {
        return true;
    }

    /** @return 无 */
    void Apply() const
    {
    }
};
