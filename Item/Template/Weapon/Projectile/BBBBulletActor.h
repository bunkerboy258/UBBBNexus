
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

    /**
     * 构造 Bullet 创建碰撞 网格 拖尾与 ProjectileMovement 组件
     */
    ABBBBulletActor();

    /**
     * 激活拖尾特效 设置生命周期并绑定 Projectile 停止回调
     */
    virtual void BeginPlay() override;

    /**
     * 销毁前立即停止拖尾特效
     * @param EndPlayReason	结束原因
     */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /**
     * 初始化 Bullet 忽略对象与初速度并激活 ProjectileMovement
     * @param InitialVelocity	初始速度
     * @param IgnoredOwner	碰撞忽略的拥有者
     * @param IgnoredEquipmentActor	碰撞忽略的装备模型
     */
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

    /**
     * Projectile 停止时销毁 Bullet
     * @param ImpactResult	命中结果
     */
    UFUNCTION()
    void HandleProjectileStop(const FHitResult &ImpactResult);
};
