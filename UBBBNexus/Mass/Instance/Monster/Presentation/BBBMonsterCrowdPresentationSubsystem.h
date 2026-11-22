#pragma once

#include "Mass/EntityHandle.h"
#include "Subsystems/WorldSubsystem.h"
#include "BBBMonsterCrowdInstance.h"
#include "BBBMonsterCrowdPresentationSubsystem.generated.h"

class UAnimSequence;
class UInstancedSkinnedMeshComponent;
class USkeletalMesh;

/** 按实际身体共享的中远距离骨骼实例批次 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterCrowdPresentationSubsystem final : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UBBBMonsterCrowdPresentationSubsystem();
    virtual void Deinitialize() override;

    bool HasPresentation(FMassEntityHandle Entity) const;
    bool Present(FMassEntityHandle Entity, USkeletalMesh& Mesh, const FTransform& Target,
        int32 Sequence, float Phase, bool bFrozen, float Alpha, float DeltaSeconds, FTransform& Display);
    void EndFrame();

private:
    UInstancedSkinnedMeshComponent* GetOrCreateComponent(USkeletalMesh& Mesh);
    void RemoveInstance(const FBBBMonsterCrowdInstance& Instance);

    UPROPERTY()
    TArray<TObjectPtr<UAnimSequence>> MaleClips;

    UPROPERTY()
    TArray<TObjectPtr<UAnimSequence>> FemaleClips;

    UPROPERTY(Transient)
    TMap<TObjectPtr<USkeletalMesh>, TObjectPtr<UInstancedSkinnedMeshComponent>> Components;

    TSet<TWeakObjectPtr<USkeletalMesh>> UnsupportedBodies;
    TMap<FMassEntityHandle, FBBBMonsterCrowdInstance> Instances;
    TMap<TWeakObjectPtr<UInstancedSkinnedMeshComponent>, float> EmptySince;
    uint64 Frame = 1;
};
