#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Definition/Events/BBBEquipmentActionPresentation.h"
#include "BBBCharacterEquipmentEvents.generated.h"

/** 单次成功装备动作事件 */
USTRUCT(BlueprintType)
struct FBBBEquipmentActionEvent
{
    GENERATED_BODY()

    /** 动作类型 */
    UPROPERTY(BlueprintReadOnly)
    EBBBCharacterActionType ActionType = EBBBCharacterActionType::None;

    /** 装备配置标识 */
    UPROPERTY(BlueprintReadOnly)
    FName EquipmentId = NAME_None;

    /** 动作顺序号 */
    UPROPERTY(BlueprintReadOnly)
    int32 Sequence = 0;

    /** 动作持续时间 */
    UPROPERTY(BlueprintReadOnly)
    float DurationSeconds = 0.0f;

    /** 仅本地使用的动作表现数据 */
    UPROPERTY(BlueprintReadOnly)
    FBBBEquipmentActionPresentation Presentation;
};

/** 单次本地后坐力事件 */
USTRUCT(BlueprintType)
struct FBBBEquipmentRecoilEvent
{
    GENERATED_BODY()

    /** 后坐力二维冲量 */
    UPROPERTY(BlueprintReadOnly)
    FVector2D Impulse = FVector2D::ZeroVector;

    /** 后坐力恢复速度 */
    UPROPERTY(BlueprintReadOnly)
    float RecoverySpeed = 0.0f;
};

/** 角色装备领域本帧事件 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentEvents
{
    GENERATED_BODY()

    /** @return 本帧成功动作事件 */
    const TArray<FBBBEquipmentActionEvent> &GetActionEvents() const
    {
        return ActionEvents;
    }

    /** @return 本帧本地后坐力事件 */
    const TArray<FBBBEquipmentRecoilEvent> &GetRecoilEvents() const
    {
        return RecoilEvents;
    }

private:
    friend class FBBBCharacterEquipmentActionProcessor;
    friend class FBBBCharacterEquipmentSelectionProcessor;
    friend struct FBBBCharacterEquipmentRuntimeData;

    /** 记录成功动作 */
    void AddAction(FBBBEquipmentActionEvent Event)
    {
        ActionEvents.Add(MoveTemp(Event));
    }

    /** 记录本地后坐力 */
    void AddRecoil(FBBBEquipmentRecoilEvent Event)
    {
        RecoilEvents.Add(MoveTemp(Event));
    }

    /** 清理本帧事件 */
    void CleanFrame()
    {
        ActionEvents.Reset();
        RecoilEvents.Reset();
    }

    /** 本帧成功动作 */
    UPROPERTY()
    TArray<FBBBEquipmentActionEvent> ActionEvents;

    /** 本帧本地后坐力 */
    UPROPERTY()
    TArray<FBBBEquipmentRecoilEvent> RecoilEvents;
};
