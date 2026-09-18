#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterDiscreteJumpInput.generated.h"

/** 跳跃动作输入 */
USTRUCT(BlueprintType)
struct FBBBCharacterDiscreteJumpInput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bPressed = false;
};
