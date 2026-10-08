#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/Definitions/BBBCharacterAppearancePart.h"
#include "BBBCharacterAppearanceSnapshot.generated.h"

/** 可以发送与还原的完整外观事实 同时是本领域的选择状态 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBCharacterAppearanceSnapshot final
{
    GENERATED_BODY()
    /** 完整部件结果 */
    UPROPERTY(BlueprintReadOnly)
    TArray<FBBBCharacterAppearancePart> Parts;
    /** 整体污渍强度 */
    UPROPERTY(BlueprintReadOnly)
    float Dirt = 0.0f;
    /** 整体磨损强度 */
    UPROPERTY(BlueprintReadOnly)
    float Weathering = 0.0f;
    /** 完整结果版本 */
    UPROPERTY()
    uint64 Revision = 0;
    /** @return 是否满足外观输入与传输的结构限制 */
    bool IsValid() const;
};
