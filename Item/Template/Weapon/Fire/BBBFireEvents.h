
#pragma once
#include "CoreMinimal.h"
#include "BBBFireEvents.generated.h"

USTRUCT(BlueprintType)
struct FBBBShotEvent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "BBB|Weapon")
    FVector_NetQuantize MuzzleLocation = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category = "BBB|Weapon")
    FVector_NetQuantizeNormal ShotDirection = FVector::ForwardVector;
};
