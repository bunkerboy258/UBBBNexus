
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

    /**
     * 提交本帧开火指令
     */
    void SubmitFire()
    {
        bActivateFire = true;
    }

    /**
     * 提交本帧换弹指令
     */
    void SubmitReload()
    {
        bActivateReload = true;
    }

    /**
     * 提交本帧开火表现指令
     */
    void SubmitFirePresentation()
    {
        bPresentFire = true;
    }

    /**
     * 提交本帧换弹表现指令
     */
    void SubmitReloadPresentation()
    {
        bPresentReload = true;
    }

    /**
     * 消费本帧开火指令 读取后清除标记
     * @return 本帧是否存在待执行开火指令
     */
    bool ConsumeFire()
    {
        const bool bShouldActivateFire = bActivateFire;
        bActivateFire = false;
        return bShouldActivateFire;
    }

    /**
     * 消费本帧换弹指令 读取后清除标记
     * @return 本帧是否存在待执行换弹指令
     */
    bool ConsumeReload()
    {
        const bool bShouldActivateReload = bActivateReload;
        bActivateReload = false;
        return bShouldActivateReload;
    }

    /**
     * 消费本帧开火表现指令 读取后清除标记
     * @return 本帧是否存在待表现开火指令
     */
    bool ConsumeFirePresentation()
    {
        const bool bShouldPresentFire = bPresentFire;
        bPresentFire = false;
        return bShouldPresentFire;
    }

    /**
     * 消费本帧换弹表现指令 读取后清除标记
     * @return 本帧是否存在待表现换弹指令
     */
    bool ConsumeReloadPresentation()
    {
        const bool bShouldPresentReload = bPresentReload;
        bPresentReload = false;
        return bShouldPresentReload;
    }

    /**
     * 清理本帧全部物品指令
     */
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
