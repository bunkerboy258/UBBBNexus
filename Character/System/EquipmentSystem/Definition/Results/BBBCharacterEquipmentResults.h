#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterEquipmentResults.generated.h"

class FBBBCharacterEquipmentActionProcessor;
struct FBBBCharacterEquipmentRuntimeData;

/** 角色本帧成功完成的物品动作结果 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentResults
{
    GENERATED_BODY()

    /** @return 本帧成功开火次数 */
    int32 GetSuccessfulFireCount() const
    {
        return SuccessfulFireCount;
    }

    /** @return 本帧成功换弹次数 */
    int32 GetSuccessfulReloadCount() const
    {
        return SuccessfulReloadCount;
    }

private:

    friend class FBBBCharacterEquipmentActionProcessor;
    friend struct FBBBCharacterEquipmentRuntimeData;

    /** 记录一次成功开火 */
    void RecordSuccessfulFire()
    {
        ++SuccessfulFireCount;
    }

    /** 记录一次成功换弹 */
    void RecordSuccessfulReload()
    {
        ++SuccessfulReloadCount;
    }

    /** 清理本帧装备动作结果 */
    void CleanFrame()
    {
        SuccessfulFireCount = 0;
        SuccessfulReloadCount = 0;
    }

    /** 本帧成功开火次数 */
    UPROPERTY()
    int32 SuccessfulFireCount = 0;

    /** 本帧成功换弹次数 */
    UPROPERTY()
    int32 SuccessfulReloadCount = 0;
};
