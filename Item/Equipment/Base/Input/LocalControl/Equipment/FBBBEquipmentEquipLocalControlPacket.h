#pragma once

/** 建立装备持有关系后的表现请求 */
struct FBBBEquipmentEquipLocalControlPacket final
{
    /** @return 包内容是否合法 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 是否允许提交表现请求 */
    bool CanApply() const
    {
        return true;
    }

    /** @return 无 */
    void Apply() const
    {
    }
};
