#pragma once
#include "CoreMinimal.h"

#include "BBBCharacterAppearancePart.generated.h"

/** 角色外观的最终部件事实 */
USTRUCT(BlueprintType)
struct FBBBCharacterAppearancePart
{
    GENERATED_BODY()
    /** @param Other 比较对象 @return 是否为同一显示事实 */
    bool operator==(const FBBBCharacterAppearancePart &Other) const
    {
        return Slot == Other.Slot && ItemId == Other.ItemId && InstanceId == Other.InstanceId
            && BaseResourceId == Other.BaseResourceId && bVisible == Other.bVisible
            && bAlternateLegs == Other.bAlternateLegs && Colors == Other.Colors
            && bCamouflage == Other.bCamouflage && bPatchEnabled == Other.bPatchEnabled
            && Patch == Other.Patch;
    }

    /** 显示部件标识 */
    UPROPERTY(BlueprintReadOnly)
    FName Slot;
    /** 实际物品型号 空名称使用基础资源 */
    UPROPERTY(BlueprintReadOnly)
    FName ItemId;
    /** 此物品的实例身份 基础资源为空 */
    UPROPERTY(BlueprintReadOnly)
    FGuid InstanceId;
    /** 角色配置中的基础资源标识 */
    UPROPERTY(BlueprintReadOnly)
    FName BaseResourceId;
    /** 本机已经确定的依附显示许可 */
    UPROPERTY(BlueprintReadOnly)
    bool bVisible = true;
    /** 本机已经确定的裤腿显示样式 */
    UPROPERTY(BlueprintReadOnly)
    bool bAlternateLegs = false;
    /** 染色区域颜色 */
    UPROPERTY(BlueprintReadOnly)
    TArray<FLinearColor> Colors;
    /** 迷彩开关 */
    UPROPERTY(BlueprintReadOnly)
    bool bCamouflage = false;
    /** 此显示部件是否启用国旗贴片 */
    UPROPERTY(BlueprintReadOnly)
    bool bPatchEnabled = false;
    /** 已经确定的国旗坐标 */
    UPROPERTY(BlueprintReadOnly)
    FVector2D Patch = FVector2D::ZeroVector;

};
