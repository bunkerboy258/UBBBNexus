
#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterItemCommands.generated.h"
class FBBBItemActionExecutor;
class FBBBCharacterItemActionProcessor;
struct FBBBCharacterItemRuntimeData;

USTRUCT(BlueprintType)
struct FBBBCharacterItemCommands
{
    GENERATED_BODY()
private:
    friend class FBBBItemActionExecutor;
    friend class FBBBCharacterItemActionProcessor;
    friend struct FBBBCharacterItemRuntimeData;

    void SubmitFire()
    {
        bActivateFire = true;
    }

    void SubmitReload()
    {
        bActivateReload = true;
    }

    bool ConsumeFire()
    {
        const bool bShouldActivateFire = bActivateFire;
        bActivateFire = false;
        return bShouldActivateFire;
    }

    bool ConsumeReload()
    {
        const bool bShouldActivateReload = bActivateReload;
        bActivateReload = false;
        return bShouldActivateReload;
    }

    void CleanFrame()
    {
        bActivateFire = false;
        bActivateReload = false;
    }

    UPROPERTY()
    bool bActivateFire = false;

    UPROPERTY()
    bool bActivateReload = false;
};
