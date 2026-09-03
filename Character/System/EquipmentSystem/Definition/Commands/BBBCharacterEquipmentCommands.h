#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBCharacterEquipmentCommands.generated.h"

class FBBBCharacterEquipmentActionProcessor;
class FBBBCharacterEquipmentSelectionProcessor;
class FBBBEquipmentActionExecutor;
class FBBBEquipmentActionRestoreProcessor;
class FBBBEquipmentRestoreProcessor;
class UBBBEquipmentDefinition;
class UBBBAnimInstance;
struct FBBBCharacterEquipmentRuntimeData;

/** 角色物品领域本帧执行命令 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentCommands
{
    GENERATED_BODY()

private:

    friend class FBBBCharacterEquipmentActionProcessor;
    friend class FBBBCharacterEquipmentSelectionProcessor;
    friend class FBBBEquipmentActionExecutor;
    friend class FBBBEquipmentActionRestoreProcessor;
    friend class FBBBEquipmentRestoreProcessor;
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

    /**
     * 提交远端已确认动作
     * @param Event 远端动作事件
     */
    void SubmitRestoredAction(FBBBEquipmentActionEvent Event)
    {
        PendingRestoredActions.Add(MoveTemp(Event));
    }

    /**
     * 提交远端还原后的期望装备配置
     * @param Definition 期望装备配置
     */
    void SubmitRestoredEquipment(UBBBEquipmentDefinition &Definition)
    {
        PendingRestoredEquipment = &Definition;
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

    /** @return 本帧待恢复动作 */
    TArray<FBBBEquipmentActionEvent> ConsumeRestoredActions()
    {
        return MoveTemp(PendingRestoredActions);
    }

    /** @return 等待装备系统创建实例的远端装备配置 */
    UBBBEquipmentDefinition *ConsumeRestoredEquipment()
    {
        UBBBEquipmentDefinition *Definition = PendingRestoredEquipment;
        PendingRestoredEquipment = nullptr;
        return Definition;
    }

    /** 清理本帧全部物品命令 */
    void CleanFrame()
    {
        bActivateFire = false;
        bActivateReload = false;
        PendingRestoredActions.Reset();
        PendingRestoredEquipment = nullptr;
    }

    /** 是否存在待执行开火命令 */
    UPROPERTY()
    bool bActivateFire = false;

    /** 是否存在待执行换弹命令 */
    UPROPERTY()
    bool bActivateReload = false;

    /** 本帧待恢复动作 */
    UPROPERTY()
    TArray<FBBBEquipmentActionEvent> PendingRestoredActions;

    /** 等待装备系统创建实例的远端装备配置 */
    UPROPERTY()
    TObjectPtr<UBBBEquipmentDefinition> PendingRestoredEquipment = nullptr;

};
