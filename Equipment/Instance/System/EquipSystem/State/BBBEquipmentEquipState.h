#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pipeline/Input/Definition/BBBEquipmentInput.h"
#include "BBBEquipmentEquipState.generated.h"

class FBBBEquipmentInitializer;
class FBBBEquipmentInputProcessor;
class FBBBEquipmentEquipProcessor;
struct FBBBEquipmentRuntimeData;

/** 装备表现系统独占的数据根 */
USTRUCT()
struct FBBBEquipmentEquipState
{
    GENERATED_BODY()

private:
    friend class FBBBEquipmentInitializer;
    friend class FBBBEquipmentInputProcessor;
    friend class FBBBEquipmentEquipProcessor;
    friend struct FBBBEquipmentRuntimeData;

    UPROPERTY()
    TArray<FBBBEquipmentInput> Inputs;
};
