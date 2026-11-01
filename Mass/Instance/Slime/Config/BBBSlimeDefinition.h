#pragma once

#include "Engine/DataAsset.h"
#include "BBBSlimeDefinition.generated.h"

class UMassEntityConfigAsset;
class UMaterialInterface;

/** 喷吐粘液的飞行 地面接触与生命周期规则 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBSlimeDefinition final : public UDataAsset
{
    GENERATED_BODY()

public:
    /** 粘液实体模板 */
    UPROPERTY(EditAnywhere, Category = "粘液", meta = (DisplayName = "实体模板"))
    TObjectPtr<UMassEntityConfigAsset> EntityConfig;
    /** 粘液飞行初速度 */
    UPROPERTY(EditAnywhere, Category = "粘液", meta = (DisplayName = "飞行速度", Units = "cm/s", ClampMin = "1"))
    float Speed = 1000;
    /** 飞行重力比例 */
    UPROPERTY(EditAnywhere, Category = "粘液", meta = (DisplayName = "重力比例", ClampMin = "0"))
    float GravityScale = 0.6f;
    /** 飞行碰撞半径 */
    UPROPERTY(EditAnywhere, Category = "粘液", meta = (DisplayName = "飞行碰撞半径", Units = "cm", ClampMin = "1"))
    float FlightRadius = 12;
    /** 地面影响半径 */
    UPROPERTY(EditAnywhere, Category = "粘液", meta = (DisplayName = "区域半径", Units = "cm", ClampMin = "1"))
    float Radius = 250;
    /** 落地后的持续秒数 */
    UPROPERTY(EditAnywhere, Category = "粘液", meta = (DisplayName = "地面持续时间", Units = "s", ClampMin = "0.1"))
    float Lifetime = 8;
    /** 最大飞行秒数 */
    UPROPERTY(EditAnywhere, Category = "粘液", meta = (DisplayName = "最长飞行时间", Units = "s", ClampMin = "0.1"))
    float FlightLifetime = 5;
    /** 实际接触时每秒生命损失 */
    UPROPERTY(EditAnywhere, Category = "粘液", meta = (DisplayName = "每秒伤害", ClampMin = "0"))
    float DamagePerSecond = 20;
    /** 实际接触时的速度倍率 */
    UPROPERTY(EditAnywhere, Category = "粘液", meta = (DisplayName = "移动速度比例", ClampMin = "0.1", ClampMax = "1"))
    float SpeedScale = 0.5f;
    /** 全世界同时存在的粘液数量上限 */
    UPROPERTY(EditAnywhere, Category = "粘液", meta = (DisplayName = "世界数量上限", ClampMin = "1", ClampMax = "128"))
    int32 WorldLimit = 32;
    /** 同一喷吐者仍然存活的区域上限 */
    UPROPERTY(EditAnywhere, Category = "粘液", meta = (DisplayName = "单体数量上限", ClampMin = "1", ClampMax = "32"))
    int32 SourceLimit = 4;
    /** 绿色粘液的表现材质 */
    UPROPERTY(EditAnywhere, Category = "表现", meta = (DisplayName = "粘液材质"))
    TObjectPtr<UMaterialInterface> Material;
    /** @return 当前飞行与接触规则是否合法 */
    bool IsValid() const;
};
