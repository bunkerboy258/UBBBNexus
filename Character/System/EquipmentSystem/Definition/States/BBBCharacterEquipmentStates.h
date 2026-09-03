#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/System/BBBEquipmentSystem.h"
#include "BBBCharacterEquipmentStates.generated.h"

class FBBBCharacterDefaultEquipmentInitializer;
class FBBBCharacterEquipmentSelectionProcessor;
class FBBBCharacterEquipmentActionProcessor;
class FBBBEquipmentRestoreProcessor;
class FBBBEquipmentSelectionExecutor;

/** 角色装备持续动作状态 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentActionState
{
    GENERATED_BODY()

    /** @return 当前持续动作 */
    EBBBCharacterActionType GetActiveAction() const
    {
        return ActiveAction;
    }

    /** @return 当前是否存在持续动作 */
    bool IsActive() const
    {
        return ActiveAction != EBBBCharacterActionType::None;
    }

    /** @return 动作开始时间 */
    float GetStartTimeSeconds() const
    {
        return StartTimeSeconds;
    }

    /** @return 动作持续时间 */
    float GetDurationSeconds() const
    {
        return DurationSeconds;
    }

    /** @return 当前动作顺序号 */
    int32 GetSequence() const
    {
        return Sequence;
    }

private:
    friend class FBBBCharacterEquipmentActionProcessor;
    friend class FBBBCharacterEquipmentSelectionProcessor;

    /**
     * 启动持续动作
     * @param InAction 动作类型
     * @param InStartTimeSeconds 开始时间
     * @param InDurationSeconds 持续时间
     * @param InSequence 动作顺序号
     */
    void Begin(
        EBBBCharacterActionType InAction,
        float InStartTimeSeconds,
        float InDurationSeconds,
        int32 InSequence,
        float InMagazineRemoveNormalizedTime = 0.0f,
        float InMagazineSpawnNormalizedTime = 0.0f)
    {
        ActiveAction = InAction;
        StartTimeSeconds = InStartTimeSeconds;
        DurationSeconds = FMath::Max(InDurationSeconds, 0.01f);
        Sequence = InSequence;
        MagazineRemoveNormalizedTime = FMath::Clamp(InMagazineRemoveNormalizedTime, 0.0f, 1.0f);
        MagazineSpawnNormalizedTime = FMath::Clamp(InMagazineSpawnNormalizedTime, 0.0f, 1.0f);
        bMagazineRemoved = false;
        bMagazineSpawned = false;
    }

    /** 清除持续动作 */
    void Reset()
    {
        ActiveAction = EBBBCharacterActionType::None;
        StartTimeSeconds = 0.0f;
        DurationSeconds = 0.0f;
        Sequence = 0;
        MagazineRemoveNormalizedTime = 0.0f;
        MagazineSpawnNormalizedTime = 0.0f;
        bMagazineRemoved = false;
        bMagazineSpawned = false;
    }

    UPROPERTY()
    EBBBCharacterActionType ActiveAction = EBBBCharacterActionType::None;

    UPROPERTY()
    float StartTimeSeconds = 0.0f;

    UPROPERTY()
    float DurationSeconds = 0.0f;

    UPROPERTY()
    int32 Sequence = 0;

    UPROPERTY()
    float MagazineRemoveNormalizedTime = 0.0f;

    UPROPERTY()
    float MagazineSpawnNormalizedTime = 0.0f;

    UPROPERTY()
    bool bMagazineRemoved = false;

    UPROPERTY()
    bool bMagazineSpawned = false;
};

/** 角色拥有的装备及快捷访问绑定 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentInventoryState
{
    GENERATED_BODY()

    /** 角色拥有的全部装备实例 */
    UPROPERTY(BlueprintReadOnly)
    TArray<TObjectPtr<UBBBEquipmentInstance>> Slots;

    /** 直接引用装备实例的快捷操作槽位 */
    UPROPERTY(BlueprintReadOnly)
    TArray<TObjectPtr<UBBBEquipmentInstance>> QuickAccessBindings;
};

/** 角色期望与当前主手装备状态 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentState
{
    GENERATED_BODY()

    /** @return 角色期望装备的主手实例 */
    UBBBEquipmentInstance *GetDesiredMainHandInstance() const
    {
        return DesiredMainHandInstance;
    }

    /** @return 角色当前装备的主手实例 */
    UBBBEquipmentInstance *GetActiveMainHandInstance() const
    {
        return ActiveMainHandInstance;
    }

    /** @return 当前装备是否仍在执行装备过渡 */
    bool IsEquipping() const
    {
        return ActionState.GetActiveAction() == EBBBCharacterActionType::Equip;
    }

    /** @return 当前是否处于换弹动作 */
    bool IsReloading() const
    {
        return ActionState.GetActiveAction() == EBBBCharacterActionType::Reload;
    }

    /** @return 当前装备持续动作状态 */
    const FBBBCharacterEquipmentActionState &GetActionState() const
    {
        return ActionState;
    }

private:
    friend class FBBBCharacterDefaultEquipmentInitializer;
    friend class FBBBCharacterEquipmentSelectionProcessor;
    friend class FBBBCharacterEquipmentActionProcessor;
    friend class FBBBEquipmentRestoreProcessor;
    friend class FBBBEquipmentSelectionExecutor;

    /** 角色期望装备的唯一实例 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBEquipmentInstance> DesiredMainHandInstance = nullptr;

    /** 角色当前装备的唯一实例 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBEquipmentInstance> ActiveMainHandInstance = nullptr;

    /** 当前装备持续动作 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    FBBBCharacterEquipmentActionState ActionState;

    /** 下一个本地动作顺序号 */
    int32 NextActionSequence = 1;
};
