#pragma once

#include "CoreMinimal.h"
#include "BBBSlimePresentation.generated.h"

class UNiagaraSystem;
class UMaterialInterface;

/** 黏液四段表现的共享资源与预算 不改变弹道或接触规则 */
USTRUCT(BlueprintType)
struct FBBBSlimePresentation final
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "资源", meta = (DisplayName = "飞行黏液材质"))
    TObjectPtr<UMaterialInterface> FlightMaterial;
    UPROPERTY(EditAnywhere, Category = "资源", meta = (DisplayName = "地面黏液材质"))
    TObjectPtr<UMaterialInterface> GroundMaterial;
    UPROPERTY(EditAnywhere, Category = "资源", meta = (DisplayName = "口部喷出"))
    TObjectPtr<UNiagaraSystem> MouthSystem;
    UPROPERTY(EditAnywhere, Category = "资源", meta = (DisplayName = "飞行拖尾"))
    TObjectPtr<UNiagaraSystem> FlightSystem;
    UPROPERTY(EditAnywhere, Category = "资源", meta = (DisplayName = "撞击泼溅"))
    TObjectPtr<UNiagaraSystem> ImpactSystem;
    UPROPERTY(EditAnywhere, Category = "资源", meta = (DisplayName = "地面气泡"))
    TObjectPtr<UNiagaraSystem> GroundSystem;

    UPROPERTY(EditAnywhere, Category = "颜色", meta = (DisplayName = "黏液深色"))
    FLinearColor MainColor = FLinearColor(.025f, .11f, .006f, 1);
    UPROPERTY(EditAnywhere, Category = "颜色", meta = (DisplayName = "黏液亮色"))
    FLinearColor SecondaryColor = FLinearColor(.14f, .27f, .025f, 1);
    UPROPERTY(EditAnywhere, Category = "飞行", meta = (DisplayName = "液团前向与横向比例"))
    FVector FlightShape = FVector(1.35f, .72f, .72f);
    UPROPERTY(EditAnywhere, Category = "飞行", meta = (DisplayName = "拖尾宽度比例", ClampMin = "0.1", ClampMax = "4"))
    float TrailWidth = .35f;
    UPROPERTY(EditAnywhere, Category = "飞行", meta = (DisplayName = "拖尾寿命", Units = "s", ClampMin = "0.05", ClampMax = "1"))
    float TrailLifetime = .2f;
    UPROPERTY(EditAnywhere, Category = "地面", meta = (DisplayName = "铺开时间", Units = "s", ClampMin = "0.05", ClampMax = "2"))
    float SpreadSeconds = .35f;
    UPROPERTY(EditAnywhere, Category = "地面", meta = (DisplayName = "消退时间", Units = "s", ClampMin = "0.1", ClampMax = "3"))
    float FadeSeconds = 1;
    UPROPERTY(EditAnywhere, Category = "预算", meta = (DisplayName = "粒子密度比例", ClampMin = "0.1", ClampMax = "1"))
    float ParticleScale = 1;
    UPROPERTY(EditAnywhere, Category = "预算", meta = (DisplayName = "最远特效距离", Units = "cm", ClampMin = "500", ClampMax = "15000"))
    float MaximumDistance = 6000;

    /** @return 资源与有限数值是否完整 */
    bool IsValid() const
    {
        for (float Value : {MainColor.R, MainColor.G, MainColor.B, MainColor.A,
            SecondaryColor.R, SecondaryColor.G, SecondaryColor.B, SecondaryColor.A})
        {
            if (!FMath::IsFinite(Value) || Value < 0 || Value > 4)
            {
                return false;
            }
        }
        return FlightMaterial && GroundMaterial && MouthSystem && FlightSystem && ImpactSystem && GroundSystem
            && !FlightShape.ContainsNaN()
            && FlightShape.GetMin() > 0 && FlightShape.GetMax() <= 4
            && FMath::IsFinite(TrailWidth) && TrailWidth >= .1f && TrailWidth <= 4
            && FMath::IsFinite(TrailLifetime) && TrailLifetime >= .05f && TrailLifetime <= 1
            && FMath::IsFinite(SpreadSeconds) && SpreadSeconds >= .05f && SpreadSeconds <= 2
            && FMath::IsFinite(FadeSeconds) && FadeSeconds >= .1f && FadeSeconds <= 3
            && FMath::IsFinite(ParticleScale) && ParticleScale >= .1f && ParticleScale <= 1
            && FMath::IsFinite(MaximumDistance) && MaximumDistance >= 500 && MaximumDistance <= 15000;
    }
};
