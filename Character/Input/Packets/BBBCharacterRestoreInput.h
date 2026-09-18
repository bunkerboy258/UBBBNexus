#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterEquipmentInput.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/AimSystem/Definition/States/BBBAimStates.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/LocomotionSystem/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBCharacterRestoreInput.generated.h"
class UBBBEquipmentDefinition;
struct FBBBCharacterRuntimeData;

/** 已完成因果计算的还原事实 */
USTRUCT()
struct FBBBCharacterRestoreInput
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<UBBBEquipmentDefinition> Equipment = nullptr;

    UPROPERTY()
    TArray<FBBBEquipmentActionEvent> Actions;

    bool bEquipmentChanged = false;
    TOptional<FBBBAimRuntimeState> Aim;
    TOptional<EBBBCharacterGait> Gait;

    /** @return 当前是否处于镜像还原模式 */
    bool CanApply(const FBBBCharacterRuntimeData &Data) const;

    /** @param Data 角色黑板 */
    void Apply(FBBBCharacterRuntimeData &Data) const;
};
