#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterMontagePlaybackState.generated.h"

class UAnimMontage;

/**
 *
 * 角色当前由引擎播放的蒙太奇记录
 */
USTRUCT()
struct FBBBCharacterMontagePlaybackState
{
    GENERATED_BODY()

    UPROPERTY(Transient)
    TObjectPtr<UAnimMontage> Montage = nullptr;

    uint64 Revision = 0;
};
