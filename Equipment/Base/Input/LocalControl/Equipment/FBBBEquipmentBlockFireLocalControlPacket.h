#pragma once

/** 装备动画禁止开火请求 具体装备维护限制 */
struct FBBBEquipmentBlockFireLocalControlPacket final
{
    /** @return 包内容是否合法 */
    bool IsValid() const { return true; }
    /** @return 是否允许提交请求 */
    bool CanApply() const { return true; }
    /** 基座请求不持有具体装备状态 @return 无 */
    void Apply() const {}
};
