#pragma once

#include "GameFramework/Actor.h"

#include "MonsterBasicActor.generated.h"

class UMonsterPresentationComponent;
class USkeletalMeshComponent;
class USphereComponent;

/** 由 Mass Representation 临时生成的小怪骨骼表现 Actor */
UCLASS(BlueprintType)
class ABBB_EVAC_API AMonsterBasicActor final : public AActor
{
    GENERATED_BODY()

public:
    /** 创建小怪表现 Actor 及其组件 */
    AMonsterBasicActor();

    /**
     * 将 UE 伤害写入关联 Mass 实体的受伤事件
     * @param DamageAmount       本次收到的伤害值
     * @param DamageEvent        伤害事件对象
     * @param EventInstigator    造成伤害的控制器
     * @param DamageCauser       造成伤害的 Actor
     * @return 被接受的伤害值
     */
    virtual float TakeDamage(
        float DamageAmount,
        FDamageEvent const& DamageEvent,
        AController* EventInstigator,
        AActor* DamageCauser) override;

    /** @return 小怪骨骼表现组件 */
    UFUNCTION(BlueprintPure, Category = "Monster|Presentation")
    USkeletalMeshComponent* GetMonsterMesh() const;

    /** @return 小怪表现状态组件 */
    UFUNCTION(BlueprintPure, Category = "Monster|Presentation")
    UMonsterPresentationComponent* GetMonsterPresentation() const;

    /** @param Radius 小怪球形碰撞体半径 */
    void SetCollisionRadius(float Radius);

private:
    /** 小怪使用的骨骼网格组件 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Presentation", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USkeletalMeshComponent> MonsterMesh;

    /** 小怪用于查询和阻挡的球形碰撞组件 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Collision", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USphereComponent> MonsterCollision;

    /** 小怪状态与动画表现组件 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Presentation", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMonsterPresentationComponent> MonsterPresentation;
};
