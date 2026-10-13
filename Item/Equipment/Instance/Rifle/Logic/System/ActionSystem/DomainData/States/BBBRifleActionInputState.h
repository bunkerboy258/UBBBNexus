#pragma once

#include "CoreMinimal.h"

/** 步枪动作领域本帧待处理输入与镜像结果 */
struct FBBBRifleActionInputState final
{
    /** 本帧是否收到装备表现请求 */
    bool bEquipRequested = false;

    /** 本帧是否收到持有者操作许可 */
    bool bActionPermissionReceived = false;

    /** 持有者允许装备继续操作的当前结果 */
    bool bActionsAllowed = true;

    /** 本帧是否收到动画禁止开火请求 */
    bool bBlockFireRequested = false;

    /** 本帧是否收到动画允许开火请求 */
    bool bAllowFireRequested = false;

    /** 本帧是否收到开火请求 */
    bool bPrimaryRequested = false;

    /** 本帧是否收到换弹请求 */
    bool bReloadRequested = false;

    /** 本帧是否收到装入弹匣通知 */
    bool bLoadMagazineRequested = false;

    /** 本帧是否收到结束换弹通知 */
    bool bInterruptReloadRequested = false;

private:
    friend struct FBBBRifleActionDomainState;

    FBBBRifleActionInputState() = default;
};
