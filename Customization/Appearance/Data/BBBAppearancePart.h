#pragma once

#include "CoreMinimal.h"
#include "BBBAppearancePart.generated.h"

/** 一个部位的模型选择与材质参数 作为存档和传输记录使用 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBAppearancePart
{
    GENERATED_BODY()

    /** 蓝图配置的部位名称 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName Slot;

    /** 配置表行名 空名称表示移除此可选部件 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName Item;

    /** 各染色区域的颜色 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FLinearColor> Colors;

    /** 是否使用迷彩 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCamouflage = false;

    /** 徽章图集坐标 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector2D Patch = FVector2D::ZeroVector;

    bool operator==(const FBBBAppearancePart &Other) const
    {
        return Slot == Other.Slot && Item == Other.Item && Colors == Other.Colors
            && bCamouflage == Other.bCamouflage && Patch == Other.Patch;
    }
};
