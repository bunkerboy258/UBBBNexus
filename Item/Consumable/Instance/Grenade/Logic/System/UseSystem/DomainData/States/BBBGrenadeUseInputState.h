#pragma once
#include "CoreMinimal.h"

/** 手雷Use领域持久状态 */
struct FBBBGrenadeUseInputState final
{
    /** 本帧主行为请求 */
    bool bPrimary = false;
    /** 本帧装备请求 */
    bool bEquip = false;
    /** 本帧释放通知 */
    bool bRelease = false;
    /** 本帧结束通知 */
    bool bFinish = false;
    /** 是否收到许可 */
    bool bHasPermission = false;
    /** 本帧许可结果 */
    bool bAllowed = false;
};
