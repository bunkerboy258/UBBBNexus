#pragma once
#include "CoreMinimal.h"
/** 手雷当前使用结果 不保存动作历史 */
struct FBBBGrenadeNetworkMessage final
{
    /** 消息顺序由公共载体赋值 */
    uint64 Revision = 0;
    /** 已开始的使用序号 */
    int32 UseSerial = 0;
    /** 已完成的释放序号 */
    int32 ReleaseSerial = 0;
    /** 当前是否处于使用动作 */
    bool bUsing = false;
    /** 当前手持网格是否已释放 */
    bool bReleased = false;
    /** 最近成立的释放变换 */
    FTransform LaunchTransform = FTransform::Identity;
    /** @return 结果数据是否有效 */
    bool IsValid() const
    {
        return Revision > 0 && UseSerial >= 0 && ReleaseSerial >= 0
            && ReleaseSerial <= UseSerial && !LaunchTransform.ContainsNaN();
    }
};
