#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterItemActionResults.generated.h"

class FBBBCharacterItemActionProcessor;
struct FBBBCharacterItemRuntimeData;

/** 保存角色本帧通过装备成功完成的动作 */
USTRUCT(BlueprintType)
struct FBBBCharacterItemActionResults
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

    friend class FBBBCharacterItemActionProcessor;
    friend struct FBBBCharacterItemRuntimeData;

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
