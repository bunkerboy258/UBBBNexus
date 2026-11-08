#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/Definitions/BBBCharacterAppearanceDisplayPart.h"
#include "BBBCharacterAppearanceDisplayState.generated.h"

/** 角色外观的本地显示执行状态 */
USTRUCT(BlueprintType)
struct FBBBCharacterAppearanceDisplayState
{
    GENERATED_BODY()

    /** 给蓝图的完整机械组装结果 */
    UPROPERTY(BlueprintReadOnly)
    TArray<FBBBCharacterAppearanceDisplayPart> Parts;
    /** 可用组件执行的基础回退缓存 */
    UPROPERTY()
    TArray<FBBBCharacterAppearanceDisplayPart> FallbackParts;
    /** 已完成资源和材质准备的选择版本 */
    UPROPERTY()
    uint64 PreparedRevision = 0;
    /** 已完整成功应用的选择版本 */
    UPROPERTY()
    uint64 AppliedRevision = 0;
    /** 最近尝试的版本 用于同一事实只报警一次 */
    UPROPERTY()
    uint64 AttemptedRevision = 0;
    /** 失败后允许重新准备和应用的世界时间 */
    UPROPERTY()
    float RetryAfterTime = 0.0f;
    /** 最近一次蓝图显示执行结果 */
    UPROPERTY(BlueprintReadOnly)
    bool bApplied = false;
    /** 最近一次基础回退是否已完成机械应用 */
    UPROPERTY(BlueprintReadOnly)
    bool bFallbackApplied = false;

};
