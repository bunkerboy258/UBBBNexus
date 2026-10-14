#pragma once
#include "Components/ActorComponent.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Network/BBBThrowableExplosionFact.h"
#include "BBBThrowableNetworkComponent.generated.h"
/** 终态可靠桥接组件 不维护运动状态 */
UCLASS()
class ABBB_EVAC_API UBBBThrowableNetworkComponent final : public UActorComponent
{
    GENERATED_BODY()
public:
    UBBBThrowableNetworkComponent();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& Out) const override;
    /** @param Fact	房主终态结果 @return 无 */
    void Publish(const FBBBThrowableExplosionFact& Fact);
    /** @param Fact	房主产生的终态 @return 无 */
    UFUNCTION(NetMulticast, Reliable)
    void MulticastExplosion(const FBBBThrowableExplosionFact& Fact);
private:
    UFUNCTION()
    void OnRep_Explosion();
    UPROPERTY(ReplicatedUsing = OnRep_Explosion)
    FBBBThrowableExplosionFact Explosion;
};
