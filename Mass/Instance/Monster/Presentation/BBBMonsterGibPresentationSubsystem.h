#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "BBBMonsterGibPresentationSubsystem.generated.h"

class UStaticMeshComponent;
class UPoseableMeshComponent;
class USkeletalMeshComponent;
class USkeletalMesh;
class UStaticMesh;
class UBBBMonsterSoundPresentationDefinition;

/** 世界共享的断肢表现池 不持有伤害事实且不创建独立更新 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterGibPresentationSubsystem final : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    /**
     * @param Source	当前僵尸骨架 仅在生成时读取姿势
     * @param Part	封闭静态部件与刚体形状
     * @param PosePart	对应外观的精简蒙皮部件
     * @param Bone	断开骨骼
     * @param Direction	当前有效受击方向
     * @param Sounds	当前外观落地声配置 槽位释放后不保留
     * @return 是否分配到表现预算
     */
    bool Emit(USkeletalMeshComponent& Source, UStaticMesh& Part, USkeletalMesh& PosePart, FName Bone,
        const FVector& Direction, UBBBMonsterSoundPresentationDefinition* Sounds);

    /**
     * @param Now	当前世界时间
     * @return 批量推进本世界部件 无返回值
     */
    void AdvancePresentation(float Now);

    /** @return 当前模拟部件数量 */
    int32 GetSimulatingCount() const;

    /** @return 当前静止残留数量 */
    int32 GetSettledCount() const;

    /** @return 已分配且可复用的组件槽位总数 */
    int32 GetAllocatedCount() const { return Pieces.Num(); }

    /** @return 世界销毁时释放池内组件 无返回值 */
    virtual void Deinitialize() override;

private:
    /** 池内刚体 独立于原僵尸的回收 */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UStaticMeshComponent>> Pieces;

    /** 只在近处生成时复制一次姿势 不运行动画图 */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UPoseableMeshComponent>> Poses;

    /** 当前部件落地声配置 槽位释放时不保留 */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UBBBMonsterSoundPresentationDefinition>> SoundConfigs;

    /** 当前组件的分配时间 负数表示空闲槽位 */
    TArray<float> BornAt;

    /** 当前组件连续稳定的开始时间 */
    TArray<float> StableSince;

    /** 每世界每引擎帧只推进一次 */
    uint64 AdvancedFrame = MAX_uint64;

    /**
     * @param Index	需要交还的池槽位
     * @return 停止物理与渲染 不销毁复用组件 无返回值
     */
    void ReleaseSlot(int32 Index);

    /** @return 按不可见 距离和年龄选择可回收的静止槽位 没有则负一 */
    int32 FindSettledRecycleSlot() const;
};
