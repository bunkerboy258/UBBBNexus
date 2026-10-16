#pragma once
#include "CoreMinimal.h"

/** 手雷Network领域持久状态 */
struct FBBBGrenadeNetworkObservationState final
{
    /** 最近已发布的使用序号 */
    int32 UseSerial = INDEX_NONE;
    /** 最近已发布的释放序号 */
    int32 ReleaseSerial = INDEX_NONE;
    /** 最近已发布的网格隐藏结果 */
    bool bHide = false;
    /** 最近已发布的动作结果 */
    bool bUsing = false;
    /** 发送结果的顺序 */
    uint64 Revision = 0;
};
