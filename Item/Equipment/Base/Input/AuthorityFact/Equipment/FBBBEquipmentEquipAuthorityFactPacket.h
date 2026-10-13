#pragma once

/** 权威装备持有关系成立后的表现请求 */
struct FBBBEquipmentEquipAuthorityFactPacket final
{
    /** @return 输入数据是否合法 */
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
