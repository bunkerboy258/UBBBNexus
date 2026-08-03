
#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterItemCommands.generated.h"
class FBBBItemActionExecutor;
class FBBBCharacterItemActionProcessor;
class FBBBFireRestoreProcessor;
class FBBBReloadRestoreProcessor;
struct FBBBCharacterItemRuntimeData;

USTRUCT(BlueprintType)
struct FBBBCharacterItemCommands
{
    GENERATED_BODY()
private:
    friend class FBBBItemActionExecutor;
    friend class FBBBCharacterItemActionProcessor;
    friend class FBBBFireRestoreProcessor;
    friend class FBBBReloadRestoreProcessor;
    friend struct FBBBCharacterItemRuntimeData;

    void SubmitFire()
    {
        bActivateFire = true;
    }

    void SubmitReload()
    {
        bActivateReload = true;
    }

    void SubmitFirePresentation()
    {
        bPresentFire = true;
    }

    void SubmitReloadPresentation()
    {
        bPresentReload = true;
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

    bool ConsumeFirePresentation()
    {
        const bool bShouldPresentFire = bPresentFire;
        bPresentFire = false;
        return bShouldPresentFire;
    }

    bool ConsumeReloadPresentation()
    {
        const bool bShouldPresentReload = bPresentReload;
        bPresentReload = false;
        return bShouldPresentReload;
    }

    void CleanFrame()
    {
        bActivateFire = false;
        bActivateReload = false;
        bPresentFire = false;
        bPresentReload = false;
    }

    UPROPERTY()
    bool bActivateFire = false;

    UPROPERTY()
    bool bActivateReload = false;

    UPROPERTY()
    bool bPresentFire = false;

    UPROPERTY()
    bool bPresentReload = false;
};
