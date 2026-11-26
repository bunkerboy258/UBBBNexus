#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/Definitions/BBBCharacterReadabilityPrimitive.h"
#include "BBBCharacterReadabilityState.generated.h"

/** 只在本机存在的角色可读性机械状态 */
USTRUCT()
struct FBBBCharacterReadabilityState
{
    GENERATED_BODY()
    /** 生命周期结束时需要恢复的部件属性 */
    UPROPERTY(Transient)
    TArray<FBBBCharacterReadabilityPrimitive> Primitives;
};
