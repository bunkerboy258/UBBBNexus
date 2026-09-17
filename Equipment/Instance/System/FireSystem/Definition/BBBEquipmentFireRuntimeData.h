#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pipeline/Input/Definition/BBBEquipmentInput.h"
#include "BBBEquipmentFireRuntimeData.generated.h"

class FBBBEquipmentInitializer;
class FBBBEquipmentInputProcessor;
class FBBBEquipmentFireProcessor;
class FBBBEquipmentReloadProcessor;
class FBBBEquipmentAnimationFactProcessor;
class FBBBEquipmentEquipProcessor;
struct FBBBEquipmentRuntimeData;

/** 开火及弹药系统独占的数据根 */
USTRUCT()
struct FBBBEquipmentFireRuntimeData
{
    GENERATED_BODY()

private:
    friend class FBBBEquipmentInitializer;
    friend class FBBBEquipmentInputProcessor;
    friend class FBBBEquipmentFireProcessor;
    friend class FBBBEquipmentReloadProcessor;
    friend class FBBBEquipmentAnimationFactProcessor;
    friend class FBBBEquipmentEquipProcessor;
    friend struct FBBBEquipmentRuntimeData;

    UPROPERTY()
    TArray<FBBBEquipmentInput> Inputs;

    UPROPERTY()
    int32 LoadedAmmo = 0;

    UPROPERTY()
    int32 AmmoCapacity = 0;

    UPROPERTY()
    float LastFireTimeSeconds = -1000.0f;

    UPROPERTY()
    int32 FireSequence = 0;
};
