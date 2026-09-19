#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBEquipmentActionFact.h"
#include "BBBCharacterEquipmentEvents.generated.h"

/** 角色装备领域本帧事件 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentEvents
{
    GENERATED_BODY()

    /** @return 本帧成功动作事件 */
    const TArray<FBBBEquipmentActionFact> &GetActionEvents() const
    {
        return ActionEvents;
    }

    /**
     * 记录成功动作
     * @param Fact	已确认动作事实
     */
    void AddAction(FBBBEquipmentActionFact Fact)
    {
        ActionEvents.Add(MoveTemp(Fact));
    }

private:
    friend class FBBBCharacterEquipmentActionProcessor;
    friend class FBBBCharacterEquipmentSelectionProcessor;
    friend struct FBBBCharacterEquipmentRuntimeData;

    /** 清理本帧事件 */
    void CleanFrame()
    {
        ActionEvents.Reset();
    }

    /** 本帧成功动作 */
    UPROPERTY()
    TArray<FBBBEquipmentActionFact> ActionEvents;
};
