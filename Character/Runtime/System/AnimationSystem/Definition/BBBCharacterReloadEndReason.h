#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterReloadEndReason.generated.h"

/** 换弹播放结束的原因 */
UENUM(BlueprintType)
enum class EBBBCharacterReloadEndReason : uint8
{
    /** 装填完成 */
    Loaded,

    /** 播放被打断 */
    Interrupted,

    /** 播放失败 */
    PlaybackFailed
};
