#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/EngineTypes.h"
#include "BBBProjectileDefinition.generated.h"

class UMassEntityConfigAsset;
class UNiagaraDataChannelAsset;
class UNiagaraSystem;

/** 描述一类本地实体弹丸的弹道、伤害与实体模板 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBProjectileDefinition final : public UDataAsset
{
    GENERATED_BODY()

public:
    /** 弹丸使用的Mass实体模板与表现配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Projectile")
    TObjectPtr<UMassEntityConfigAsset> EntityConfig = nullptr;

    /** 枪口初速度，单位厘米每秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Projectile", meta = (ClampMin = "1.0", ForceUnits = "cm/s"))
    float InitialSpeedCmPerSecond = 50000.0f;

    /** 弹丸最长存活时间，单位秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Projectile", meta = (ClampMin = "0.01", ForceUnits = "s"))
    float MaximumLifetimeSeconds = 5.0f;

    /** 球形连续扫掠半径，设为零时使用线段检测，单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Projectile", meta = (ClampMin = "0.0", ForceUnits = "cm"))
    float CollisionRadiusCm = 2.0f;

    /** 每次命中造成的基础点伤害 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Projectile", meta = (ClampMin = "0.0"))
    float BaseDamage = 20.0f;

    /** 命中目标后可继续穿过的目标数量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Projectile", meta = (ClampMin = "0", ClampMax = "32"))
    int32 MaximumPenetrations = 0;

    /** 每穿透一个目标后剩余伤害的倍率 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Projectile", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float PenetrationDamageMultiplier = 0.65f;

    /** 弹丸执行连续碰撞查询使用的碰撞通道 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Projectile")
    TEnumAsByte<ECollisionChannel> CollisionChannel = ECC_Pawn;

    /** 批量曳光与命中光效的数据通道 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Projectile|Presentation")
    TObjectPtr<UNiagaraDataChannelAsset> PresentationChannel = nullptr;

    /** 子弹共享曳光系统 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Projectile|Presentation")
    TObjectPtr<UNiagaraSystem> PresentationSystem = nullptr;

    /** 飞行光段长度 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Projectile|Presentation", meta = (ClampMin = "1.0", ForceUnits = "cm"))
    float TracerLengthCm = 1000.0f;

    /** 飞行光段宽度 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Projectile|Presentation", meta = (ClampMin = "0.1", ForceUnits = "cm"))
    float TracerWidthCm = 2.5f;

    /** 飞行光段的线性发光颜色 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Projectile|Presentation")
    FLinearColor TracerColor = FLinearColor(20.0f, 8.0f, 1.0f, 1.0f);

    /** @return 配置资产和运行参数是否完整有效 */
    bool IsValid() const;
};
