#pragma once
#include "CoreMinimal.h"
#include "BBBWorldDropConfig.generated.h"
class UBBBItemDefinition;
class UMaterialInterface;
/** 世界掉落物生成与共用显示配置 */
USTRUCT(BlueprintType)
struct FBBBWorldDropConfig final
{
    GENERATED_BODY()
    /** 关卡摆放时的物品定义 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "掉落物", meta = (DisplayName = "物品定义", ToolTip = "本份掉落物使用的已有物品定义"))
    TObjectPtr<UBBBItemDefinition> Definition = nullptr;
    /** 本份掉落物的数量 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "掉落物", meta = (DisplayName = "数量", ClampMin = "1", ClampMax = "1000000", ToolTip = "必须完整进入背包的物品数量"))
    int32 Quantity = 1;
    /** 生成时的世界速度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "掉落物", meta = (DisplayName = "初始速度", ToolTip = "生成后的世界空间速度 单位厘米每秒"))
    FVector InitialVelocity = FVector::ZeroVector;
    /** 物理球体半径 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "掉落物", meta = (DisplayName = "碰撞半径", ClampMin = "1", ToolTip = "世界物理碰撞球半径 单位厘米"))
    float Radius = 8.0f;
    /** 白色图标的显示高度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "显示", meta = (DisplayName = "图标高度", ClampMin = "1", ToolTip = "白色缩略图高度 单位厘米 宽度保持贴图比例"))
    float IconHeight = 22.0f;
    /** 图标中心离地距离 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "显示", meta = (DisplayName = "图标抬高", ClampMin = "1", ToolTip = "图标中心相对掉落位置的高度 单位厘米"))
    float IconLift = 42.0f;
    /** 光柱高度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "显示", meta = (DisplayName = "光柱高度", ClampMin = "1", ToolTip = "世界光柱高度 单位厘米"))
    float BeamHeight = 140.0f;
    /** 光柱半径 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "显示", meta = (DisplayName = "光柱半径", ClampMin = "0.1", ToolTip = "世界光柱半径 单位厘米"))
    float BeamRadius = 1.5f;
    /** 接受图标纹理参数的共用材质 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "显示", meta = (DisplayName = "图标材质", ToolTip = "透明白色图标材质 使用 IconTexture 纹理参数"))
    TSoftObjectPtr<UMaterialInterface> IconMaterial = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/_Project/Characters/BBBC_UA/Items/WorldDrop/Materials/M_WorldDropIcon.M_WorldDropIcon")));
    /** 光柱共用材质 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "显示", meta = (DisplayName = "光柱材质", ToolTip = "光柱的半透明自发光材质"))
    TSoftObjectPtr<UMaterialInterface> BeamMaterial = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/_Project/Characters/BBBC_UA/Items/WorldDrop/Materials/M_WorldDropBeam.M_WorldDropBeam")));
};
