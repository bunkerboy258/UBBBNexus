#pragma once

#include "CoreMinimal.h"

class UBBBMonsterBloodPresentationDefinition;

/** 当前仍在飞行的代表性血滴 碰撞或超时后立即释放 */
struct FBBBMonsterBloodDroplet final
{
    TWeakObjectPtr<const UBBBMonsterBloodPresentationDefinition> Settings;
    FVector Position = FVector::ZeroVector;
    FVector Velocity = FVector::ZeroVector;
    float Age = 0.0f;
    uint32 Seed = 0;
    bool bFine = false;
};
