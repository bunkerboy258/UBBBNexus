#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterDiscreteFireInput.generated.h"

/** 开火动作输入 */
USTRUCT(BlueprintType)
struct FBBBCharacterDiscreteFireInput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bPressed = false;
};
