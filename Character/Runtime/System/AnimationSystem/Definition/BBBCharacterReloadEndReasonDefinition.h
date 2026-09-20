#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterReloadEndReasonDefinition.generated.h"

/** 换弹播放结束的原因 */
UENUM(BlueprintType)
enum class EBBBCharacterReloadEndReasonDefinition : uint8
{
    /** 装填完成 */
    Loaded,

    /** 播放被打断 */
    Interrupted,

    /** 播放失败 */
    PlaybackFailed
};
