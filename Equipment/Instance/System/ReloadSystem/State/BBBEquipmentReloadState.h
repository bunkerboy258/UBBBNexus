#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pipeline/Input/Definition/BBBEquipmentInput.h"
#include "BBBEquipmentReloadState.generated.h"

class FBBBEquipmentInitializer;
class FBBBEquipmentInputProcessor;
class FBBBEquipmentReloadProcessor;
class FBBBEquipmentFireProcessor;
struct FBBBEquipmentRuntimeData;

/** 换弹系统独占的数据根 */
USTRUCT()
struct FBBBEquipmentReloadState
{
    GENERATED_BODY()

public:
    /** @return 当前装备是否正在换弹 */
    bool IsReloading() const
    {
        return bIsReloading;
    }

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
