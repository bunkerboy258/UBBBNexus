#pragma once
#include "CoreMinimal.h"
class USkeletalMesh;
class UMaterialInterface;
#include "BBBAppearanceResource.generated.h"

/** 角色外观的共享静态资源 */
USTRUCT(BlueprintType)
struct FBBBAppearanceResource
{
    GENERATED_BODY()

    /** 对应的显示部件 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "显示部位"))
    FName Part;
    /** 显示模型 空引用表示空部件 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "模型"))
    TSoftObjectPtr<USkeletalMesh> Mesh;
    /** 保持模型形状时跟随的骨骼 空名称表示使用角色蒙皮姿势 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "刚性跟随骨骼"))
    FName RigidAttachBone;
    /** 当前裤腿样式 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "裤腿样式"))
    FName LegStyle;
    /** 鞋子要求的裤腿样式 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "要求裤腿"))
    FName RequiredLegStyle;
    /** 基础长裤的另一显示样式 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "替代裤腿模型"))
    TSoftObjectPtr<USkeletalMesh> AlternateLegMesh;
    /** 一件附件预设内部的挂点与模型 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "附件"))
    TMap<FName, TSoftObjectPtr<USkeletalMesh>> Attachments;
    /** 依附物显示所需的穿戴位置 空名称表示不限制 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "依附穿戴位置"))
    FName RequiredWearSlot;
    /** 此模型是否提供国旗贴片位置 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "支持国旗"))
    bool bSupportsPatch = false;
    /** 国旗图集坐标 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "国旗坐标"))
    FVector2D Patch = FVector2D::ZeroVector;
    /** 染色区域对应的材质参数 按颜色顺序配置 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "染色参数"))
    TArray<FName> ColorParameters;
    /** 迷彩开关参数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "迷彩参数"))
    FName CamouflageParameter;
    /** 贴片开关参数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "贴片开关参数"))
    FName PatchEnabledParameter;
    /** 图集坐标向量参数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "贴片坐标参数"))
    FName PatchParameter;
    /** 国旗独占的材质槽名 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "国旗材质槽"))
    FName PatchMaterialSlot;
    /** 不显示国旗时使用的透明裁剪材质 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "隐藏国旗材质"))
    TSoftObjectPtr<UMaterialInterface> HiddenPatchMaterial;
    /** 污渍参数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "污渍参数"))
    FName DirtParameter;
    /** 磨损参数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "磨损参数"))
    FName WeatheringParameter;

};
