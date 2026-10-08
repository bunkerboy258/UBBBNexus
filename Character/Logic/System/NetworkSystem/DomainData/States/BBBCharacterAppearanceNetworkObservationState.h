#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterAppearanceNetworkObservationState.generated.h"
/** 外观结果的独立发送基准 */
USTRUCT()
struct FBBBCharacterAppearanceNetworkObservationState final
{
    GENERATED_BODY()
    /** 已经发送的既成事实版本 */
    UPROPERTY()
    uint64 Revision = 0;
};
