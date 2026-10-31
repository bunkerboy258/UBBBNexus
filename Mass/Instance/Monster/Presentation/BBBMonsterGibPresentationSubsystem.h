#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "BBBMonsterGibPresentationSubsystem.generated.h"

class UStaticMeshComponent;
class UPoseableMeshComponent;
class USkeletalMeshComponent;
class USkeletalMesh;
class UStaticMesh;
class UBBBMonsterSoundPresentationDefinition;
class UBBBMonsterPresentationDefinition;
class UPrimitiveComponent;
class UPhysicalMaterial;
class AActor;
struct FHitResult;
struct FReferenceSkeleton;
struct FBBBMonsterBloodImpact;

/** 世界共享的断肢表现池 不持有伤害事实且不创建独立更新 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterGibPresentationSubsystem final : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    /**
     * @param Impact	当前命中接触及表现来源
     * @param Settings	共享外观与碎肉参数
     * @return 实际生成的小肉块数量
     */
    int32 EmitFlesh(const FBBBMonsterBloodImpact& Impact, const UBBBMonsterPresentationDefinition& Settings);

    /** @return 当前活动的小肉块数量 */
    int32 GetFleshCount() const;

    /** @param Out	当前活动的小肉块组件 @return 只读采样用列表 无返回值 */
    void GetFleshPieces(TArray<UStaticMeshComponent*>& Out) const;

    /**
     * @param Source	明确的调试来源
     * @return 清理该来源的小肉块 无返回值
     */
    void ClearFleshForSource(const AActor& Source);

    /**
     * @param SourceSkeleton	身体骨架
     * @param ComponentPose	身体当前组件空间姿态
     * @param PartSkeleton	残肢骨架 不要求与身体排列一致
     * @param LocalPose	按残肢骨架顺序输出的局部姿态
     * @return 所有残肢骨骼是否匹配且变换有限
     */
    static bool BuildDetachedPose(const FReferenceSkeleton& SourceSkeleton,
        TConstArrayView<FTransform> ComponentPose, const FReferenceSkeleton& PartSkeleton,
        TArray<FTransform>& LocalPose);

    /** @param WorldType\t实际世界类型 @return 正式世界和隔离的编辑预览均使用同一残肢池 */
    virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override
    {
        return WorldType == EWorldType::EditorPreview || Super::DoesSupportWorldType(WorldType);
    }

    /**
     * @param Source	当前僵尸骨架 仅在生成时读取姿势
     * @param Part	封闭静态部件与刚体形状
     * @param PosePart	对应外观的精简蒙皮部件
     * @param Bone	断开骨骼
     * @param Direction	当前有效受击方向
     * @param Velocity	断开时的 Mass 移动速度
     * @param Seed	本次断开表现的稳定种子
     * @param Sounds	当前外观落地声配置 槽位释放后不保留
     * @return 是否分配到表现预算
     */
    bool Emit(USkeletalMeshComponent& Source, UStaticMesh& Part, USkeletalMesh& PosePart, FName Bone,
        const FVector& Direction, const FVector& Velocity, uint32 Seed, const UBBBMonsterPresentationDefinition& Settings);

    /**
     * @param Now	当前世界时间
     * @return 批量推进本世界部件 无返回值
     */
    void AdvancePresentation(float Now);

    /** @return 当前模拟部件数量 */
    int32 GetSimulatingCount() const;

    /** @return 当前静止残留数量 */
    int32 GetSettledCount() const;

    /** @return 活动槽位中已发生真实地面接触的数量 */
    int32 GetLandedCount() const;

    /** @return 已分配且可复用的组件槽位总数 */
    int32 GetAllocatedCount() const { return Pieces.Num(); }

    /** @return 世界销毁时释放池内组件 无返回值 */
    virtual void Deinitialize() override;

private:
    /** 池槽是否属于小肉块 */
    TArray<bool> FleshPieces;
    /** 严重毁伤碎肉使用更长间隔 防止一次连射重复堆叠 */
    TArray<bool> MajorFleshPieces;
    /** 首次落地时间 未落地为负值 */
    TArray<float> LandedAt;
    /** 碎肉来源的实体出生身份 */
    TArray<FGuid> FleshInstances;
    /** 调试来源只作限频与隔离清理 */
    TArray<TWeakObjectPtr<AActor>> FleshSources;

    /**
     * @param bFlesh	是否申请小肉块槽位
     * @return 可复用槽位或负一
     */
    int32 AcquireSlot(bool bFlesh);

    /** 活动池槽保留共享配置引用 不复制完整资源表 */
    UPROPERTY(Transient)
    TArray<TObjectPtr<const UBBBMonsterPresentationDefinition>> PresentationConfigs;
    /** 池内刚体 独立于原僵尸的回收 */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UStaticMeshComponent>> Pieces;

    /** 只在近处生成时复制一次姿势 不运行动画图 */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UPoseableMeshComponent>> Poses;

    /** 当前部件落地声配置 槽位释放时不保留 */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UBBBMonsterSoundPresentationDefinition>> SoundConfigs;

    /** 碎肢共享的地面摩擦与轻微回弹材质 */
    UPROPERTY(Transient)
    TObjectPtr<UPhysicalMaterial> ContactMaterial;

    /** 每个活动槽位是否已经发生首次地面接触 */
    TArray<bool> LandingPlayed;

    /** 当前组件的分配时间 负数表示空闲槽位 */
    TArray<float> BornAt;

    /** 当前组件连续稳定的开始时间 */
    TArray<float> StableSince;

    /** 同一世界时间只推进一次 离屏预览可独立推进物理时间 */
    float AdvancedTime = -1;

    /**
     * @param Index	需要交还的池槽位
     * @return 停止物理与渲染 不销毁复用组件 无返回值
     */
    void ReleaseSlot(int32 Index);

    /** @return 按不可见 距离和年龄选择可回收的静止槽位 没有则负一 */
    int32 FindSettledRecycleSlot() const;

    /**
     * @param HitComponent	发生真实物理接触的池内刚体
     * @param OtherActor	接触的环境载体
     * @param OtherComponent	接触的环境组件
     * @param NormalImpulse	本次碰撞冲量
     * @param Hit	当前物理接触位置与法线
     * @return 仅在首次有效落地播放声音 无返回值
     */
    UFUNCTION()
    void OnPieceHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
        FVector NormalImpulse, const FHitResult& Hit);
};
