#pragma once

#include "CoreMinimal.h"

/** 步枪镜像只还原当前结果 不重放动作历史 */
struct FBBBRifleNetworkStatePacket final
{
    /** 当前装填弹量 */
    int32 LoadedAmmo = 0;

    /** 当前累计开火次数 */
    int32 FireSequence = 0;

    /** 当前换弹流程序号 */
    int32 ReloadSequence = 0;

    /** 当前是否正在换弹 */
    bool bIsReloading = false;

    /** 当前弹匣是否已经卸下 */
    bool bMagazineDetached = false;
};
