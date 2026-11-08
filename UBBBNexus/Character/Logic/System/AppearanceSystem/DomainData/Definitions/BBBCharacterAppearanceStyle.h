#pragma once
#include "CoreMinimal.h"

#include "BBBCharacterAppearanceStyle.generated.h"

/** 角色外观的实例外观参数 */
USTRUCT(BlueprintType)
struct FBBBCharacterAppearanceStyle
{
    GENERATED_BODY()

    /** 实例身份 为空时描述基础部件 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGuid InstanceId;
    /** 基础显示部件 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName Slot;
    /** 区域染色 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FLinearColor> Colors;
    /** 迷彩开关 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCamouflage = false;

};
