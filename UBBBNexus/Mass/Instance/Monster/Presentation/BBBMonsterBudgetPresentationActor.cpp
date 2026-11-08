#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterBudgetPresentationActor.h"

#include "IAnimationBudgetAllocator.h"
#include "SkeletalMeshComponentBudgeted.h"

ABBBMonsterBudgetPresentationActor::ABBBMonsterBudgetPresentationActor(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<USkeletalMeshComponentBudgeted>(TEXT("MonsterMesh")))
{
    USkeletalMeshComponentBudgeted* const Mesh = Cast<USkeletalMeshComponentBudgeted>(GetMonsterMesh());
    if (!ensureMsgf(Mesh, TEXT("[BBBMonsterBudget]Budget mesh subobject substitution failed")))
    {
        return;
    }

    Mesh->SetAutoRegisterWithBudgetAllocator(true);
    Mesh->SetAutoCalculateSignificance(true);
}

void ABBBMonsterBudgetPresentationActor::BeginPlay()
{
    Super::BeginPlay();
    IAnimationBudgetAllocator* const Budget = IAnimationBudgetAllocator::Get(GetWorld());
    if (!ensureMsgf(Budget, TEXT("[BBBMonsterBudget]Animation budget allocator is unavailable")))
    {
        return;
    }

    Budget->SetEnabled(true);
}
