
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBBulletActor.generated.h"
class UNiagaraComponent;
class UNiagaraSystem;
class UProjectileMovementComponent;
class USphereComponent;
class UStaticMeshComponent;

UCLASS(Blueprintable)
class ABBB_EVAC_API ABBBBulletActor : public AActor
{
    GENERATED_BODY()
public:


    ABBBBulletActor();


    virtual void BeginPlay() override;


    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


    UFUNCTION(BlueprintCallable, Category = "BBB|Bullet")
    void InitializeBullet(FVector InitialVelocity, AActor *IgnoredOwner, AActor *IgnoredEquipmentActor);
protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Bullet")
    TObjectPtr<USphereComponent> CollisionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Bullet")
    TObjectPtr<UStaticMeshComponent> VisualMeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Bullet")
    TObjectPtr<UNiagaraComponent> TrailComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Bullet")
    TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Bullet", meta = (ClampMin = "0.0"))
    float LifeSeconds = 5.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Bullet|Trail")
    TObjectPtr<UNiagaraSystem> TrailEffect = nullptr;


    UFUNCTION()
    void HandleProjectileStop(const FHitResult &ImpactResult);
};
