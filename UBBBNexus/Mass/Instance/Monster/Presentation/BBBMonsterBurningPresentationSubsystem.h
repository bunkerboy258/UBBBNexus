#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "BBBMonsterBurningPresentationSubsystem.generated.h"

class UNiagaraDataChannelAsset;
class USkeletalMeshComponent;

/**
 * 全局共享尸群燃烧表现子系统：
 * 彻底替代逐怪 OverlayMaterial 与逐怪 NiagaraComponent，
 * 通过单一 Niagara Data Channel（固定 2 DrawCall）对可见燃烧僵尸进行多骨骼贴体采样、速度拖尾与全局预算控载。
 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterBurningPresentationSubsystem final : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override
    {
        return WorldType == EWorldType::EditorPreview || Super::DoesSupportWorldType(WorldType);
    }

    /** 提前激活共享燃烧通道，避免首帧引燃卡顿 */
    void Prepare();

    /**
     * 每帧由 Mass 表现处理器登记一只可见的燃烧僵尸身体
     * @param BodyMesh          僵尸骨骼网格（支持站立、踉跄、爬行与刚倒地燃烧尸体）
     * @param Velocity          僵尸当前世界速度（赋予火苗与火星惯性拖尾）
     * @param VariationSeed     僵尸个体随机种子（错峰采样）
     * @param ViewerDistSq      与最近观察者的距离平方（距离分级控载）
     */
    void QueueBurningBody(const USkeletalMeshComponent& BodyMesh, const FVector& Velocity,
        uint32 VariationSeed, float ViewerDistSq);

    /**
     * 本帧登记结束后统一按全局硬预算批量写入共享 Niagara Data Channel
     * @param DeltaSeconds      本帧时间步长
     */
    void Flush(float DeltaSeconds);

    virtual void Deinitialize() override;

private:
    struct FBurningEntry
    {
        TWeakObjectPtr<const USkeletalMeshComponent> Mesh;
        FVector Velocity = FVector::ZeroVector;
        uint32 Seed = 0;
        float DistSq = 0.0f;
    };

    UPROPERTY(Transient)
    TObjectPtr<UNiagaraDataChannelAsset> BurningChannel;

    TArray<FBurningEntry> QueuedBodies;
    TMap<uint32, float> SpawnAccumulators;
    uint32 FrameSequence = 0;
    uint64 FlushedFrame = MAX_uint64;
    bool bPrepared = false;
};
