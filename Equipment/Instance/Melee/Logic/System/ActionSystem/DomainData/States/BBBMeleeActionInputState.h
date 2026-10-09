#pragma once
#include "CoreMinimal.h"
/** 解析完成后等待攻击领域消费的当前帧请求 */
struct FBBBMeleeActionInputState final
{
    /** 当前帧主行为请求 */
    bool bPrimary = false;

    /** 当前帧收到持有者操作许可 */
    bool bActionPermissionReceived = false;

    /** 持有者允许继续装备操作 */
    bool bActionsAllowed = true;
    /** 生命周期开始标识 */
    TArray<int32> BeginActions;
    /** 生命周期结束标识 */
    TArray<int32> EndActions;
    /** 开窗标识 */
    TArray<int32> BeginContacts;
    /** 关窗标识 */
    TArray<int32> EndContacts;
    /** 本帧收到持有表现请求 */
    bool bEquip = false;

private:
    friend struct FBBBMeleeActionDomainState;
    FBBBMeleeActionInputState() = default;
};
