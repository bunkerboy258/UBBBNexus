#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/Definitions/BBBCharacterAppearanceStyle.h"
#include "BBBCharacterAppearanceStyleState.generated.h"

/** 角色外观的参数与基础选择状态 */
USTRUCT(BlueprintType)
struct FBBBCharacterAppearanceStyleState
{
    GENERATED_BODY()

    /** 同一实例跨位置保留的外观参数 */
    UPROPERTY()
    TArray<FBBBCharacterAppearanceStyle> Styles;
    /** 基础部件选择 只指向角色静态配置 */
    UPROPERTY()
    TMap<FName, FName> BaseParts;
    /** 整体污渍强度 */
    UPROPERTY()
    float Dirt = 0.0f;
    /** 整体磨损强度 */
    UPROPERTY()
    float Weathering = 0.0f;
    /** 参数改变时递增 */
    UPROPERTY()
    uint64 Revision = 0;

};
