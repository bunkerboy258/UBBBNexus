#include "BBBCharacterPhysicalAnimationComponent.h"

#include "Components/SkeletalMeshComponent.h"

void UBBBCharacterPhysicalAnimationComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    if (USkeletalMeshComponent* SkeletalMesh = GetSkeletalMesh(); IsValid(SkeletalMesh))
    {
        // 在引擎物理动画进入场景写锁前收束并行骨骼任务
        SkeletalMesh->GetBoneSpaceTransformsView();
    }

    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
