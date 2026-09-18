#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterDiscreteReloadInput.generated.h"

/** 换弹动画反馈阶段 */
UENUM(BlueprintType)
enum class EBBBCharacterReloadPhase : uint8
{
    None,
    DetachMagazine,
    LoadMagazine,
    Interrupted
};

/** 换弹播放结束的原因 */
UENUM(BlueprintType)
enum class EBBBCharacterReloadEndReason : uint8
{
    Loaded,
    Interrupted,
    PlaybackFailed
};

/** 换弹动作和动画阶段输入 */
USTRUCT(BlueprintType)
struct FBBBCharacterDiscreteReloadInput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bPressed = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBBBCharacterReloadPhase Phase = EBBBCharacterReloadPhase::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Sequence = INDEX_NONE;
};
