#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterInputPackets.h"
#include "BBBCharacterInputRuntimeData.generated.h"

class FBBBCharacterInput;
class FBBBCharacterParseSystem;
class FBBBCharacterInputProcessor;

/** 角色输入入口的持续采样和离散收件箱 */
USTRUCT()
struct FBBBCharacterInputRuntimeData
{
    GENERATED_BODY()

private:
    friend class FBBBCharacterInput;
    friend class FBBBCharacterParseSystem;
    friend class FBBBCharacterInputProcessor;

    UPROPERTY()
    FBBBCharacterContinuousInput Continuous;

    UPROPERTY()
    TArray<FBBBCharacterDiscreteInput> PendingDiscrete;

    UPROPERTY()
    TArray<FBBBCharacterRestoreDiscreteInput> PendingRestoreDiscrete;
};
