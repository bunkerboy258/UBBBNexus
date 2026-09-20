#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipmentActionFact.h"
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
     * 提交本帧装备副操作状态
     * @param bActive  副操作是否处于按下状态
     * @return 无
     */
    void SubmitSecondary(const bool bActive)
    {
        bSecondaryActive = bActive;
        bSecondarySubmitted = true;
    }

    /**
     * 提交远端已确认动作
     * @param Fact	远端动作事实
     */
    void SubmitFact(FBBBEquipmentActionFact Fact)
    {
        PendingFacts.Add(MoveTemp(Fact));
    }

    /**
     * 提交远端还原后的期望装备配置
     * @param Definition	期望装备配置
     */
    void SubmitEquipmentState(UBBBEquipmentDefinition &Definition)
    {
        PendingEquipmentState = &Definition;
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

    /**
     * 消费本帧装备副操作状态
     * @param bOutActive   接收副操作是否按下
     * @return 本帧是否提交过副操作状态
     */
    bool ConsumeSecondary(bool &bOutActive)
    {
        if (!bSecondarySubmitted)
        {
            return false;
        }

        bOutActive = bSecondaryActive;
        bSecondarySubmitted = false;
        return true;
    }

    /** @return 本帧待恢复动作 */
    TArray<FBBBEquipmentActionFact> ConsumeFacts()
    {
        return MoveTemp(PendingFacts);
    }

    /** @return 等待装备系统创建实例的远端装备配置 */
    UBBBEquipmentDefinition *ConsumeEquipmentState()
    {
        UBBBEquipmentDefinition *Definition = PendingEquipmentState;
        PendingEquipmentState = nullptr;
        return Definition;
    }

    /** 清理本帧全部物品命令 */
    void CleanFrame()
    {
        bActivateFire = false;
        bActivateReload = false;
        bSecondarySubmitted = false;
        PendingFacts.Reset();
        PendingEquipmentState = nullptr;
    }

    /** 是否存在待执行开火命令 */
    UPROPERTY()
    bool bActivateFire = false;

    /** 是否存在待执行换弹命令 */
    UPROPERTY()
    bool bActivateReload = false;

    /** 本帧是否提交了副操作状态 */
    UPROPERTY()
    bool bSecondarySubmitted = false;

    /** 本帧最后一次副操作状态 */
    UPROPERTY()
    bool bSecondaryActive = false;

    /** 本帧待恢复动作 */
    UPROPERTY()
    TArray<FBBBEquipmentActionFact> PendingFacts;

    /** 等待装备系统创建实例的远端装备配置 */
    UPROPERTY()
    TObjectPtr<UBBBEquipmentDefinition> PendingEquipmentState = nullptr;

};
