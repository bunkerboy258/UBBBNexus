#pragma once

/** 具体装备解释的中断换弹请求 */
struct FBBBEquipmentInterruptReloadLocalControlPacket final
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
