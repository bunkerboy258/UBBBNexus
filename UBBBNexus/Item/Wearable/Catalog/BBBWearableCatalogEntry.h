#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/BBBItemDefinition.h"
#include "BBBWork/UBBBNexus/Item/Wearable/Config/BBBAppearanceResource.h"
#include "BBBWearableCatalogEntry.generated.h"

/** 单一物品类别的静态注册 */
USTRUCT(BlueprintType)
struct FBBBWearableCatalogEntry final
{
    GENERATED_BODY()
    /** 注册的静态物品定义 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "物品定义"))
    TObjectPtr<UBBBItemDefinition> Definition = nullptr;

    /** 外侧静态显示资源 角色负责解释穿戴关系 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "外观资源"))
    FBBBAppearanceResource Appearance;
};
