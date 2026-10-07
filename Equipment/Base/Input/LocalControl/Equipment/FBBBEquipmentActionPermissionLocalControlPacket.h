#pragma once

#include "CoreMinimal.h"

/** 持有者提交的操作许可 不解释具体装备行为 */
struct FBBBEquipmentActionPermissionLocalControlPacket final
{
    /** 是否允许装备启动或继续自身操作 */
    TArray<bool> Permissions;

    /** @return 至少有一份操作许可 */
    bool IsValid() const
    {
        return !Permissions.IsEmpty();
    }

    /** @return 公共请求可以转交具体装备 */
    bool CanApply() const
    {
        return true;
    }

    /** @return 无 公共请求不写具体装备状态 */
    void Apply() const
    {
    }
};
