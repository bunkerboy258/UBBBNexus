#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterMontagePlaybackEntry.generated.h"

class UAnimMontage;

/**
 *
 * 角色当前由引擎播放的蒙太奇记录
 */
USTRUCT()
struct FBBBCharacterMontagePlaybackEntry final
{
    GENERATED_BODY()

    /** 已提交给动画实例的蒙太奇 */
    UPROPERTY(Transient)
    TObjectPtr<UAnimMontage> Montage = nullptr;

    /** 对应槽位请求的播放修订号 */
    uint64 Revision = 0;
};
