#pragma once

#include "CoreMinimal.h"

#include "Subsystems/WorldSubsystem.h"
#include "BBBProjectilePresentationSubsystem.generated.h"

class UBBBProjectileDefinition;
class UNiagaraComponent;
class UNiagaraDataChannelAsset;
class UNiagaraSystem;
class UStaticMesh;
class UInstancedStaticMeshComponent;
class UDecalComponent;
class UMaterialInterface;
struct FBBBProjectileImpact;

/** 当前世界的弹丸模板准备与共享曳光生命周期 不生成发射事实 */
UCLASS()
class ABBB_EVAC_API UBBBProjectilePresentationSubsystem final : public UTickableWorldSubsystem
{
    GENERATED_BODY()
public:
    /** @param Definition 当前世界实际使用的发射配置 @return 无 */
    void Prepare(UBBBProjectileDefinition &Definition);
    /** @param Channel 发射通道 @param System 曳光系统 @return 共享组件是否已准备 */
    bool IsReady(const UNiagaraDataChannelAsset *Channel, const UNiagaraSystem *System) const;
    /** @param Channel 当前曳光通道 @param System 当前曳光系统 @return 无 为直接生成的弹丸排队准备 */
    void PrepareTracer(UNiagaraDataChannelAsset *Channel, UNiagaraSystem *System);
    /** @param Batches 当前活动弹体的模型变换 @return 无 只更新已有显示对象 */
    void UpdateMeshes(const TMap<UStaticMesh *, TArray<FTransform>> &Batches);
    /** @param Impacts 本帧命中批次 @return 无 仅在静态表面复用 64 槽环形池更新弹孔 */
    void PublishBulletHoles(TConstArrayView<FBBBProjectileImpact> Impacts);
    /** @param Origin 爆炸中心 @param Endpoints 破片划出终点列表 @return 无 渲染瞬时白色破片划出线 */
    void SpawnFragStreaks(const FVector &Origin, TConstArrayView<FVector> Endpoints);
    /** @return 已预加载与预热的燃烧弹地面焦黑贴花材质 */
    UMaterialInterface *GetScorchDecalMaterial();
    virtual void Initialize(FSubsystemCollectionBase &Collection) override;
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override;
    virtual TStatId GetStatId() const override;
    virtual void Deinitialize() override;
private:
    struct FFragStreak
    {
        FVector Origin = FVector::ZeroVector;
        FVector Endpoint = FVector::ZeroVector;
        float Age = 0.0f;
        float Duration = 0.095f;
    };
    /** @return 无 每帧最多准备一个弹体组件 */
    void PrepareNextMesh();
    void UpdateFragStreaks(float DeltaTime);
    UPROPERTY(Transient)
    TArray<TObjectPtr<UStaticMesh>> PendingMeshes;
    UPROPERTY(Transient)
    TMap<TObjectPtr<UStaticMesh>, TObjectPtr<UInstancedStaticMeshComponent>> Meshes;
    /** 有界于实际获得的配置 每帧只准备一个模板 */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UBBBProjectileDefinition>> Pending;
    TSet<TWeakObjectPtr<UBBBProjectileDefinition>> Prepared;
    UPROPERTY(Transient)
    TObjectPtr<UNiagaraComponent> Tracer;
    UPROPERTY(Transient)
    TObjectPtr<UNiagaraDataChannelAsset> ActiveChannel;
    UPROPERTY(Transient)
    TObjectPtr<UNiagaraSystem> ActiveSystem;
    /** 固定容量 64 槽弹孔/破片撕裂贴花环形池 */
    UPROPERTY(Transient)
    TObjectPtr<UMaterialInterface> BulletHoleMaterial;
    UPROPERTY(Transient)
    TObjectPtr<UMaterialInterface> FragTearMaterial;
    UPROPERTY(Transient)
    TObjectPtr<UMaterialInterface> ScorchDecalMaterial;
    UPROPERTY(Transient)
    TArray<TObjectPtr<UDecalComponent>> BulletHolePool;
    TArray<FVector> BulletHolePositions;
    int32 NextBulletHoleSlot = 0;
    /** 单 DrawCall 破片白色划出线实例组件 */
    UPROPERTY(Transient)
    TObjectPtr<UInstancedStaticMeshComponent> FragStreakMesh;
    TArray<FFragStreak> FragStreaks;
};
