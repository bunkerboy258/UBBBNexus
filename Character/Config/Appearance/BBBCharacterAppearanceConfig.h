#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Config/Appearance/BBBAppearanceResource.h"
#include "BBBCharacterAppearanceConfig.generated.h"

/** 角色外观的基础静态配置 */
USTRUCT(BlueprintType)
struct FBBBCharacterAppearanceConfig
{
    GENERATED_BODY()

    /** 非物品资源标识与资源定义 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "基础外观资源"))
    TMap<FName, FBBBAppearanceResource> BaseResources;
    /** 部件对应的默认基础资源 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "默认基础部件"))
    TMap<FName, FName> DefaultBaseParts;
    /** 显示部位与蓝图机械组件名称的对应关系 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "显示组件名称"))
    TMap<FName, FName> ComponentNames;
    /** 默认整体污渍 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "默认污渍"))
    float Dirt = 0.0f;
    /** 默认整体磨损 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "默认磨损"))
    float Weathering = 0.0f;

};
