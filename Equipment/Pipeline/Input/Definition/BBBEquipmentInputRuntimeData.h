#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Pipeline/Input/Definition/BBBEquipmentInput.h"
#include "BBBEquipmentInputRuntimeData.generated.h"

class FBBBEquipmentExternalAPI;
class FBBBEquipmentInputProcessor;

/** 尚未进入本帧处理的外部输入 */
USTRUCT()
struct FBBBEquipmentInputRuntimeData
{
    GENERATED_BODY()

private:
    friend class FBBBEquipmentExternalAPI;
    friend class FBBBEquipmentInputProcessor;

    UPROPERTY()
    TArray<FBBBEquipmentInput> Pending;
};
