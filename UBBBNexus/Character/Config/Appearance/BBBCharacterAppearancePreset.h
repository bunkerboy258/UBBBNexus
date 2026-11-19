#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Config/Appearance/BBBCharacterAppearancePalette.h"
#include "BBBCharacterAppearancePreset.generated.h"

/** 整套角色支持的原版配色配置 */
USTRUCT(BlueprintType)
struct FBBBCharacterAppearancePreset
{
    GENERATED_BODY()

    /** 此整套网格可换色的材质槽 顺序用于还原已确定的配色 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "部位配色表"))
    TArray<FBBBCharacterAppearancePalette> Palettes;
};
