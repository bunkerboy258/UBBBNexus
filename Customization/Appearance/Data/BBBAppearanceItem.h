#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BBBAppearanceItem.generated.h"

class USkeletalMesh;
class UTexture2D;

/** 外观目录中的一项资源与搭配说明 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBAppearanceItem : public FTableRowBase
{
    GENERATED_BODY()

    /** 所属部位 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "部件栏位"))
    FName Slot;

    /** 界面名称 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "显示名称"))
    FText DisplayName;

    /** 款式选择界面显示的部件缩略图 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "缩略图"))
    TSoftObjectPtr<UTexture2D> Thumbnail;

    /** 骨骼网格 空资源表示此部位不显示 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "骨骼网格"))
    TSoftObjectPtr<USkeletalMesh> Mesh;

    /** 裤腿样式 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "腿部样式"))
    FName LegStyle;

    /** 靴子需要的裤腿样式 空名称表示不限制 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "要求的腿部样式"))
    FName RequiredLegStyle;

    /** 同款裤子的另一种裤腿样式 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "替代腿部物品"))
    FName AlternateLegItem;

    /** 附件预设 挂点名称对应骨骼网格 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "附加物"))
    TMap<FName, TSoftObjectPtr<USkeletalMesh>> Attachments;
};
