#pragma once
#include "GameFramework/Actor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/RuntimeData/BBBThrowableRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/ParseSystem/BBBThrowableParseSystem.h"
#include "BBBThrowable.generated.h"
class USphereComponent;
class UStaticMeshComponent;
class UBBBThrowableNetworkComponent;
class FBBBThrowableInitializer;
class FBBBThrowableUpdatePipeline;
class FBBBThrowableShutdown;

/** 房主权威物理投掷物 运动由 UE Chaos 与 Predictive Interpolation 管理 */
UCLASS(BlueprintType, NotBlueprintable)
class ABBB_EVAC_API ABBBThrowable final : public AActor
{
    GENERATED_BODY()
public:
    ABBBThrowable();
    /** @param Packet	投掷物独立输入 @return 无 仅贡献输入 */
    template<typename TPacket>
    void SubmitInput(TPacket&& Packet)
    {
        ParseSystem.Submit(RuntimeData, Forward<TPacket>(Packet));
    }
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& Out) const override;
    /** @return 只读静态定义 */
    const UBBBThrowableDefinition* GetDefinition() const { return Definition; }
    /** 唯一业务状态 */
    FBBBThrowableRuntimeData RuntimeData;
private:
    friend class FBBBThrowableInitializer;
    friend class FBBBThrowableLifetimeProcessor;
    friend class FBBBThrowableExplosionProcessor;
    friend class FBBBThrowableNetworkProcessor;
    friend class FBBBThrowableShutdown;
    UFUNCTION()
    void OnRep_Definition();
    UFUNCTION()
    void OnPhysicsHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComponent, FVector Impulse, const FHitResult& Hit);
    UPROPERTY(VisibleAnywhere, Category = "投掷物", meta = (DisplayName = "物理碰撞体"))
    TObjectPtr<USphereComponent> PhysicsBody;
    UPROPERTY(VisibleAnywhere, Category = "投掷物", meta = (DisplayName = "可见网格"))
    TObjectPtr<UStaticMeshComponent> Mesh;
    UPROPERTY()
    TObjectPtr<UBBBThrowableNetworkComponent> Network;
    UPROPERTY(ReplicatedUsing = OnRep_Definition)
    TObjectPtr<UBBBThrowableDefinition> Definition;
    FBBBThrowableParseSystem ParseSystem;
};
