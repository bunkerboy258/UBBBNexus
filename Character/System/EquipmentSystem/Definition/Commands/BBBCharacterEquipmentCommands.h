#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterEquipmentCommands.generated.h"

class FBBBCharacterEquipmentActionProcessor;
class FBBBFireRestoreProcessor;
class FBBBEquipmentActionExecutor;
class FBBBReloadRestoreProcessor;
class UBBBAnimInstance;
struct FBBBCharacterEquipmentRuntimeData;

/** 角色物品领域本帧执行命令 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentCommands
{
    GENERATED_BODY()

private:

    friend class FBBBCharacterEquipmentActionProcessor;
    friend class FBBBFireRestoreProcessor;
    friend class FBBBEquipmentActionExecutor;
    friend class FBBBReloadRestoreProcessor;
    friend class UBBBAnimInstance;
    friend struct FBBBCharacterEquipmentRuntimeData;

    /** 提交本帧开火命令 */
    void SubmitFire()
    {
        bActivateFire = true;
    }

    /** 提交本帧换弹命令 */
    void SubmitReload()
    {
        bActivateReload = true;
    }

    /** 提交本帧开火表现命令 */
    void SubmitFirePresentation()
    {
        bPresentFire = true;
    }

    /** 提交本帧换弹表现命令 */
    void SubmitReloadPresentation()
    {
        bPresentReload = true;
    }

    /** 提交等待消费的拔出弹匣动作 */
    void SubmitRemoveMagazine()
    {
        bPendingRemoveMagazine = true;
    }

    /** 提交等待消费的生成弹匣动作 */
    void SubmitSpawnMagazine()
    {
        bPendingSpawnMagazine = true;
    }

    /** @return 本帧是否存在待执行开火命令 */
    bool ConsumeFire()
    {
        const bool bShouldActivateFire = bActivateFire;
        bActivateFire = false;
        return bShouldActivateFire;
    }

    /** @return 本帧是否存在待执行换弹命令 */
    bool ConsumeReload()
    {
        const bool bShouldActivateReload = bActivateReload;
        bActivateReload = false;
        return bShouldActivateReload;
    }

    /** @return 本帧是否存在待表现开火命令 */
    bool ConsumeFirePresentation()
    {
        const bool bShouldPresentFire = bPresentFire;
        bPresentFire = false;
        return bShouldPresentFire;
    }

    /** @return 本帧是否存在待表现换弹命令 */
    bool ConsumeReloadPresentation()
    {
        const bool bShouldPresentReload = bPresentReload;
        bPresentReload = false;
        return bShouldPresentReload;
    }

    /** @return 是否存在等待消费的拔出弹匣动作 */
    bool ConsumeRemoveMagazine()
    {
        const bool bShouldRemoveMagazine = bPendingRemoveMagazine;
        bPendingRemoveMagazine = false;
        return bShouldRemoveMagazine;
    }

    /** @return 是否存在等待消费的生成弹匣动作 */
    bool ConsumeSpawnMagazine()
    {
        const bool bShouldSpawnMagazine = bPendingSpawnMagazine;
        bPendingSpawnMagazine = false;
        return bShouldSpawnMagazine;
    }

    /** 清理本帧全部物品命令 */
    void CleanFrame()
    {
        bActivateFire = false;
        bActivateReload = false;
        bPresentFire = false;
        bPresentReload = false;
    }

    /** 是否存在待执行开火命令 */
    UPROPERTY()
    bool bActivateFire = false;

    /** 是否存在待执行换弹命令 */
    UPROPERTY()
    bool bActivateReload = false;

    /** 是否存在待表现开火命令 */
    UPROPERTY()
    bool bPresentFire = false;

    /** 是否存在待表现换弹命令 */
    UPROPERTY()
    bool bPresentReload = false;

    /** 是否存在等待消费的拔出弹匣动作 */
    UPROPERTY()
    bool bPendingRemoveMagazine = false;

    /** 是否存在等待消费的生成弹匣动作 */
    UPROPERTY()
    bool bPendingSpawnMagazine = false;
};
