#pragma once
#include "CoreMinimal.h"
#include "BBBWorldDropConfig.generated.h"
class UBBBItemDefinition;
class UMaterialInterface;
class UStaticMesh;
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
    /** 飞行期间随物理主体移动的实体外观 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "显示", meta = (DisplayName = "空中掉落包网格", ToolTip = "仅在尚未落地时显示的实体包裹 复用现有物理主体 不创建独立装备"))
    TSoftObjectPtr<UStaticMesh> AirborneMesh = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/_ThirdParty/Environment/ApocalypticCity/StaticMeshs/Bag_sm.Bag_sm")));
    /** 实体包裹最长边的尺寸 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "显示", meta = (DisplayName = "空中掉落包尺寸", ClampMin = "1", ToolTip = "自动按网格包围盒归一化后的最长边 单位厘米"))
    float AirborneSize = 24.0f;
    /** 白色图标的显示高度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "显示", meta = (DisplayName = "图标高度", ClampMin = "1", ToolTip = "白色缩略图高度 单位厘米 宽度保持贴图比例"))
    float IconHeight = 39.0f;
    /** 长条物品图标的宽度上限 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "显示", meta = (DisplayName = "图标最大宽度", ClampMin = "1", ToolTip = "图标保持原始宽高比后允许的最大宽度 单位厘米"))
    float IconMaxWidth = 63.0f;
    /** 图标中心离地距离 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "显示", meta = (DisplayName = "图标抬高", ClampMin = "1", ToolTip = "图标中心相对掉落位置的高度 单位厘米"))
    float IconLift = 117.0f;
    /** 光柱高度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "显示", meta = (DisplayName = "光柱高度", ClampMin = "1", ToolTip = "世界光柱高度 单位厘米"))
    float BeamHeight = 105.0f;
    /** 包含柔和外沿的光束总宽度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "显示", meta = (DisplayName = "光束宽度", ClampMin = "0.1", ToolTip = "相机朝向光束的总宽度 单位厘米"))
    float BeamWidth = 12.0f;
    /** 地面柔光直径 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "显示", meta = (DisplayName = "地面微光直径", ClampMin = "1", ToolTip = "掉落位置附近柔光的总直径 单位厘米"))
    float GroundGlowDiameter = 21.0f;
    /** 落地显示的淡入时长 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "显示", meta = (DisplayName = "淡入时长", ClampMin = "0", ToolTip = "从落地到完整显示的本地淡入时长 单位秒 零表示立即显示"))
    float FadeInDuration = 0.2f;
    /** 接受图标纹理参数的共用材质 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "显示", meta = (DisplayName = "图标材质", ToolTip = "透明白色图标材质 使用 IconTexture 纹理参数"))
    TSoftObjectPtr<UMaterialInterface> IconMaterial = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/_Project/Characters/BBBC_UA/Items/WorldDrop/Materials/M_WorldDropIcon.M_WorldDropIcon")));
    /** 光柱共用材质 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "显示", meta = (DisplayName = "光柱材质", ToolTip = "光柱的半透明自发光材质"))
    TSoftObjectPtr<UMaterialInterface> BeamMaterial = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/_Project/Characters/BBBC_UA/Items/WorldDrop/Materials/M_WorldDropBeam.M_WorldDropBeam")));
    /** 仅表达落点的共用柔光材质 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "显示", meta = (DisplayName = "地面微光材质", ToolTip = "具有柔和透明边缘的地面微光材质"))
    TSoftObjectPtr<UMaterialInterface> GroundGlowMaterial = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/_Project/Characters/BBBC_UA/Items/WorldDrop/Materials/M_WorldDropGroundGlow.M_WorldDropGroundGlow")));
};
