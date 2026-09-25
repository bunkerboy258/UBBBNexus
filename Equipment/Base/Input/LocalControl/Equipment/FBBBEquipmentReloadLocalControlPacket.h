#pragma once

/** 装备本机换弹请求 */
struct FBBBEquipmentReloadLocalControlPacket final
{
    /** @return 包内容是否合法 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 是否允许提交换弹请求 */
    bool CanApply() const
    {
        return true;
    }

    /** @return 无 */
    void Apply() const
    {
    }
};
