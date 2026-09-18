#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Events/BBBEquipmentActionEvent.h"
#include "BBBCharacterEquipmentEvents.generated.h"

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

private:
    friend class FBBBCharacterInputProcessor;
    friend class FBBBCharacterInput;
    friend class FBBBCharacterEquipmentActionProcessor;
    friend class FBBBCharacterEquipmentSelectionProcessor;
    friend struct FBBBCharacterEquipmentRuntimeData;

    /** 记录成功动作 */
    void AddAction(FBBBEquipmentActionEvent Event)
    {
        ActionEvents.Add(MoveTemp(Event));
    }

    /** 清理本帧事件 */
    void CleanFrame()
    {
        ActionEvents.Reset();
    }

    /** 本帧成功动作 */
    UPROPERTY()
    TArray<FBBBEquipmentActionEvent> ActionEvents;


};
