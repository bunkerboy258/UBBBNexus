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
    /** 已尝试应用的选择版本 */
    UPROPERTY()
    uint64 AppliedRevision = 0;
    /** 最近一次蓝图显示执行结果 */
    UPROPERTY(BlueprintReadOnly)
    bool bApplied = false;

};
