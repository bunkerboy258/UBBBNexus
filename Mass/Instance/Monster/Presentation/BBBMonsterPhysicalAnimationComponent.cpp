#include "BBBMonsterPhysicalAnimationComponent.h"

#include "Components/SkeletalMeshComponent.h"

void UBBBMonsterPhysicalAnimationComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    if (USkeletalMeshComponent* SkeletalMesh = GetSkeletalMesh(); IsValid(SkeletalMesh))
    {
        // 骨骼物理混合也需要场景锁 禁止持有场景写锁时等待该任务
        SkeletalMesh->GetBoneSpaceTransformsView();
    }

    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
