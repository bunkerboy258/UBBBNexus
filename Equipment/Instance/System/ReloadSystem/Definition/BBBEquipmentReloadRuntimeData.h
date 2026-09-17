#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pipeline/Input/Definition/BBBEquipmentInput.h"
#include "BBBEquipmentReloadRuntimeData.generated.h"

class FBBBEquipmentInitializer;
class FBBBEquipmentInputProcessor;
class FBBBEquipmentReloadProcessor;
class FBBBEquipmentFireProcessor;
struct FBBBEquipmentRuntimeData;

/** 换弹系统独占的数据根 */
USTRUCT()
struct FBBBEquipmentReloadRuntimeData
{
    GENERATED_BODY()

private:
    friend class FBBBEquipmentInitializer;
    friend class FBBBEquipmentInputProcessor;
    friend class FBBBEquipmentReloadProcessor;
    friend class FBBBEquipmentFireProcessor;
    friend struct FBBBEquipmentRuntimeData;

    UPROPERTY()
    TArray<FBBBEquipmentInput> Inputs;

    UPROPERTY()
    bool bIsReloading = false;

    UPROPERTY()
    bool bMagazineDetached = false;

    UPROPERTY()
    int32 Sequence = INDEX_NONE;
};
