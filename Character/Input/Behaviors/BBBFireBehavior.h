#pragma once
#include "CoreMinimal.h"
#include "BBBFireBehavior.generated.h"

struct FBBBCharacterRuntimeData;

/** 本帧开火请求 持续按住时每帧提交一次 */
USTRUCT(BlueprintType)
struct FBBBFireInput
{
    GENERATED_BODY()
};

/** 开火行为的固定规则 */
struct FBBBFireBehavior
{
    using FInput = FBBBFireInput;
    static bool AllowsFire(bool bReloading, bool bSwitching, bool bStartingReload);
    static bool CanStart(const FInput &Input, const FBBBCharacterRuntimeData &Data);
    static void Start(const FInput &Input, FBBBCharacterRuntimeData &Data);
};
