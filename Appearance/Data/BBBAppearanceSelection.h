#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Appearance/Data/BBBAppearancePart.h"
#include "BBBAppearanceSelection.generated.h"

/** 可独立保存和传输的一套外观选择 不包含运行时组件与资源对象 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBAppearanceSelection
{
    GENERATED_BODY()

    /** 按蓝图部位配置排列的完整选择 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FBBBAppearancePart> Parts;

    /** 整体污渍强度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Dirt = 0.0f;

    /** 整体磨损强度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Weathering = 0.0f;

    /** 附件组合配置表行名 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName Attachments;

    /** @return 是否满足输入与网络边界的基本结构要求 */
    bool IsValid() const;

    bool operator==(const FBBBAppearanceSelection &Other) const
    {
        return Parts == Other.Parts && Dirt == Other.Dirt
            && Weathering == Other.Weathering && Attachments == Other.Attachments;
    }

    bool operator!=(const FBBBAppearanceSelection &Other) const
    {
        return !(*this == Other);
    }
};
