#pragma once

#include "CoreMinimal.h"
#include "BBBFacingStates.generated.h"

/** 角色当前朝向状态 */
USTRUCT(BlueprintType)
struct FBBBCharacterFacingState
{
    GENERATED_BODY()

    /** 身体是否正在追赶瞄准方向 */
    UPROPERTY(BlueprintReadOnly)
    bool bIsBodyTurning = false;

};
