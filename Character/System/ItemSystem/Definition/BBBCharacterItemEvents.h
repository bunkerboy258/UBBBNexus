
#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterItemEvents.generated.h"
class FBBBCharacterExternalAPI;
struct FBBBCharacterItemRuntimeData;

USTRUCT(BlueprintType)
struct FBBBCharacterItemEvents
{
    GENERATED_BODY()

    int32 GetFireEventCount() const
    {
        return FireEventCount;
    }

    int32 GetReloadEventCount() const
    {
        return ReloadEventCount;
    }
private:
    friend class FBBBCharacterExternalAPI;
    friend struct FBBBCharacterItemRuntimeData;

    void PublishFire()
    {
        ++FireEventCount;
    }

    void PublishReload()
    {
        ++ReloadEventCount;
    }

    void CleanFrame()
    {
        FireEventCount = 0;
        ReloadEventCount = 0;
    }

    UPROPERTY()
    int32 FireEventCount = 0;

    UPROPERTY()
    int32 ReloadEventCount = 0;
};
