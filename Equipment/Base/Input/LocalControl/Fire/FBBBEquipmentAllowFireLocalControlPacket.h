#pragma once

/** 具体装备解释的允许开火请求 */
struct FBBBEquipmentAllowFireLocalControlPacket final
{
    /** @return 无数据请求始终有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 由具体装备决定如何应用请求 */
    bool CanApply() const
    {
        return true;
    }

    /** @return 基座不写入具体装备状态 */
    void Apply() const
    {
    }
};
