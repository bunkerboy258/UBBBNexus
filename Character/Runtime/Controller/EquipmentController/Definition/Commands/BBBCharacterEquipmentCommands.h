#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBEquipmentActionFact.h"
#include "BBBCharacterEquipmentCommands.generated.h"

class FBBBCharacterEquipmentActionProcessor;
class FBBBCharacterEquipmentSelectionProcessor;

class UBBBEquipmentDefinition;
class UBBBAnimInstance;
struct FBBBCharacterEquipmentRuntimeData;

/** 角色物品领域本帧执行命令 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentCommands
{
    GENERATED_BODY()

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
     * @param Fact	远端动作事实
     */
    void SubmitRestoredAction(FBBBEquipmentActionFact Fact)
    {
        PendingRestoredActions.Add(MoveTemp(Fact));
    }

    /**
     * 提交远端还原后的期望装备配置
     * @param Definition	期望装备配置
     */
    void SubmitRestoredEquipment(UBBBEquipmentDefinition &Definition)
    {
        PendingRestoredEquipment = &Definition;
    }

    /**
     * 提交卸下弹匣阶段
     * @param Sequence	换弹序号
     */
    void SubmitDetachMagazine(const int32 Sequence)
    {
        DetachMagazineSequences.Add(Sequence);
    }

    /**
     * 提交装填弹匣阶段
     * @param Sequence	换弹序号
     */
    void SubmitLoadMagazine(const int32 Sequence)
    {
        LoadMagazineSequences.Add(Sequence);
    }

    /**
     * 提交取消换弹阶段
     * @param Sequence	换弹序号
     */
    void SubmitCancelReload(const int32 Sequence)
    {
        CancelReloadSequences.Add(Sequence);
    }

private:
    friend class FBBBCharacterEquipmentActionProcessor;
    friend class FBBBCharacterEquipmentSelectionProcessor;

    friend class UBBBAnimInstance;
    friend struct FBBBCharacterEquipmentRuntimeData;

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
    TArray<FBBBEquipmentActionFact> ConsumeRestoredActions()
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
        DetachMagazineSequences.Reset();
        LoadMagazineSequences.Reset();
        CancelReloadSequences.Reset();
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
    TArray<FBBBEquipmentActionFact> PendingRestoredActions;

    /** 等待装备系统创建实例的远端装备配置 */
    UPROPERTY()
    TObjectPtr<UBBBEquipmentDefinition> PendingRestoredEquipment = nullptr;

    /** 本帧卸下弹匣阶段序号 */
    UPROPERTY()
    TArray<int32> DetachMagazineSequences;

    /** 本帧装填弹匣阶段序号 */
    UPROPERTY()
    TArray<int32> LoadMagazineSequences;

    /** 本帧取消换弹阶段序号 */
    UPROPERTY()
    TArray<int32> CancelReloadSequences;
};
