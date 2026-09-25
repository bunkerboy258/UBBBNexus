#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BBBAppearanceItem.generated.h"

class USkeletalMesh;

/** 外观目录中的一项资源与搭配说明 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBAppearanceItem : public FTableRowBase
{
    GENERATED_BODY()

    /** 所属部位 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName Slot;

    /** 界面名称 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText DisplayName;

    /** 骨骼网格 空资源表示此部位不显示 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<USkeletalMesh> Mesh;

    /** 裤腿样式 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName LegStyle;

    /** 靴子需要的裤腿样式 空名称表示不限制 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName RequiredLegStyle;

    /** 同款裤子的另一种裤腿样式 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName AlternateLegItem;

    /** 附件预设 挂点名称对应骨骼网格 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TMap<FName, TSoftObjectPtr<USkeletalMesh>> Attachments;
};
