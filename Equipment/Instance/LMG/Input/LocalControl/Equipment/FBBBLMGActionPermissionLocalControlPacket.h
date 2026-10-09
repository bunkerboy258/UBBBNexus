#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/ActionSystem/DomainData/States/BBBLMGActionInputState.h"

/** 本类装备收到的操作许可 */
struct FBBBLMGActionPermissionLocalControlPacket final
{
    /** 本帧按提交顺序累计的操作许可 */
    TArray<bool> Permissions;

    /** @return 数据有效 */
    bool IsValid() const
    {
        return !Permissions.IsEmpty();
    }

    /** @return 许可可写入本帧状态 */
    bool CanApply() const
    {
        return true;
    }

    /** @param State	动作输入状态 @return 无 */
    void Apply(FBBBLMGActionInputState &State) const
    {
        State.bActionPermissionReceived = true;
        State.bActionsAllowed = Permissions.Last();
    }
};
