#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/EngineTypes.h"
#include "BBBProjectileDefinition.generated.h"

class UMassEntityConfigAsset;
class UNiagaraDataChannelAsset;
class UNiagaraSystem;
class UStaticMesh;

/** 描述一类本地实体弹丸的弹道 伤害与实体模板 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBProjectileDefinition final : public UDataAsset
{
    GENERATED_BODY()

public:
    /** 每发生成的独立弹丸数量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|发射", meta = (ClampMin = "1", ClampMax = "64", DisplayName = "每发弹丸数"))
    int32 ProjectilesPerShot = 1;

    /** 相对枪口正前方的散射锥半角 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|发射", meta = (ClampMin = "0.0", ClampMax = "90.0", DisplayName = "散射半角"))
    float SpreadHalfAngleDegrees = 0.0f;

    /** 世界重力倍率 零表示直线飞行 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|运动", meta = (ClampMin = "0.0", DisplayName = "重力倍率"))
    float GravityScale = 0.0f;

    /** 范围伤害半径 零表示点伤害 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|爆炸", meta = (ClampMin = "0.0", ForceUnits = "cm", DisplayName = "爆炸半径 厘米"))
    float ExplosionRadiusCm = 0.0f;

    /** 正数表示出生后延时引爆 零表示没有引信 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|爆炸", meta = (ClampMin = "0.0", ForceUnits = "s", DisplayName = "引信时间 秒"))
    float FuseSeconds = 0.0f;

    /** 范围伤害弹丸接触目标时立即引爆 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|爆炸", meta = (DisplayName = "接触引爆"))
    bool bDetonateOnImpact = true;

    /** 未引爆的弹丸接触后反弹 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|运动", meta = (DisplayName = "碰撞反弹"))
    bool bBounceOnImpact = false;

    /** 反弹后保留的速度比例 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|运动", meta = (ClampMin = "0.0", ClampMax = "1.0", DisplayName = "反弹速度倍率"))
    float BounceRestitution = 0.4f;

    /** 批量实例显示的弹体网格 为空时仅显示曳光 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|表现", meta = (DisplayName = "弹体网格"))
    TObjectPtr<UStaticMesh> Mesh = nullptr;

    /** 网格相对飞行方向的旋转 位移和缩放 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|表现", meta = (DisplayName = "网格相对变换"))
    FTransform MeshRelativeTransform = FTransform::Identity;

    /** 弹丸使用的Mass实体模板与表现配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸", meta = (DisplayName = "实体配置"))
    TObjectPtr<UMassEntityConfigAsset> EntityConfig = nullptr;

    /** 枪口初速度 单位厘米每秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸", meta = (ClampMin = "1.0", ForceUnits = "cm/s", DisplayName = "初速度 厘米每秒"))
    float InitialSpeedCmPerSecond = 50000.0f;

    /** 弹丸最长存活时间 单位秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸", meta = (ClampMin = "0.01", ForceUnits = "s", DisplayName = "最长存活时间 秒"))
    float MaximumLifetimeSeconds = 5.0f;

    /** 球形连续扫掠半径 设为零时使用线段检测 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸", meta = (ClampMin = "0.0", ForceUnits = "cm", DisplayName = "碰撞半径 厘米"))
    float CollisionRadiusCm = 2.0f;

    /** 每次命中造成的基础点伤害 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸", meta = (ClampMin = "0.0", DisplayName = "基础伤害"))
    float BaseDamage = 20.0f;

    /** 命中高耐久部位时参与混合的伤害 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸", meta = (ClampMin = "0.0", DisplayName = "耐久伤害"))
    float DurableDamage = 10.0f;

    /** 命中目标后可继续穿过的目标数量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸", meta = (ClampMin = "0", ClampMax = "32", DisplayName = "最大穿透次数"))
    int32 MaximumPenetrations = 0;

    /** 每穿透一个目标后剩余伤害的倍率 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸", meta = (ClampMin = "0.0", ClampMax = "1.0", DisplayName = "穿透伤害倍率"))
    float PenetrationDamageMultiplier = 0.65f;

    /** 弹丸执行连续碰撞查询使用的碰撞通道 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸", meta = (DisplayName = "碰撞通道"))
    TEnumAsByte<ECollisionChannel> CollisionChannel = ECC_Pawn;

    /** 批量曳光与命中光效的数据通道 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|表现", meta = (DisplayName = "表现数据通道"))
    TObjectPtr<UNiagaraDataChannelAsset> PresentationChannel = nullptr;

    /** 子弹共享曳光系统 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|表现", meta = (DisplayName = "表现特效系统"))
    TObjectPtr<UNiagaraSystem> PresentationSystem = nullptr;

    /** 当帧表面命中批次 空间岛自行启动反馈系统 不保存命中历史 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|表现", meta = (DisplayName = "命中数据通道"))
    TObjectPtr<UNiagaraDataChannelAsset> ImpactChannel = nullptr;

    /** 飞行光段长度 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|表现", meta = (ClampMin = "1.0", ForceUnits = "cm", DisplayName = "曳光长度 厘米"))
    float TracerLengthCm = 1000.0f;

    /** 飞行光段宽度 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|表现", meta = (ClampMin = "0.1", ForceUnits = "cm", DisplayName = "曳光宽度 厘米"))
    float TracerWidthCm = 2.5f;

    /** 飞行光段的线性发光颜色 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|表现", meta = (DisplayName = "曳光颜色"))
    FLinearColor TracerColor = FLinearColor(20.0f, 8.0f, 1.0f, 1.0f);

    /** @return 配置资产和运行参数是否完整有效 */
    bool IsValid() const;
};
