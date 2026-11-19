#pragma once
#include "CoreMinimal.h"

class UTexture;

#include "BBBCharacterAppearancePalette.generated.h"

/** 整套角色原有部位的材质槽与配色 */
USTRUCT(BlueprintType)
struct FBBBCharacterAppearancePalette
{
    GENERATED_BODY()

    /** 配色对应的物品穿戴位置 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "穿戴位置"))
    FName WearSlot;

    /** 整身网格上的目标材质槽 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "材质槽"))
    FName MaterialSlot;

    /** 原作者配色顺序 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "颜色贴图"))
    TArray<TSoftObjectPtr<UTexture>> Textures;

    /** 空槽显示的原版配色序号 负一使用网格材质自身的颜色 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "空槽配色"))
    int32 DefaultIndex = INDEX_NONE;
};
