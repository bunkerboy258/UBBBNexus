#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationComponent.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"
#include "Components/SkeletalMeshComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterFactAnimInstance.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterHitReactionComponent.h"
#include "IAnimationBudgetAllocator.h"
#include "SkeletalMeshComponentBudgeted.h"

void UBBBMonsterPresentationComponent::ApplyHitReaction(const FBBBMonsterHitReactionFragment& Hit)
{
    HitReaction = Hit;
    if (auto* Reaction = GetOwner()->FindComponentByClass<UBBBMonsterHitReactionComponent>())
    {
        Reaction->ApplyHitFacts(Hit, BBBMonsterBehavior != EBBBMonsterBehavior::Dead);
    }
}

void UBBBMonsterPresentationComponent::ApplyMobilityState(const bool bInCrawling, const float InProgress, const float InHalfHeight)
{
    bCrawling = bInCrawling;
    CrawlProgress = FMath::Clamp(InProgress, 0.0f, 1.0f);
    if (auto* Mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>())
    {
        FVector Offset = Mesh->GetRelativeLocation();
        Offset.Z = -InHalfHeight;
        Mesh->SetRelativeLocation(Offset);
    }
}

UBBBMonsterPresentationComponent::UBBBMonsterPresentationComponent()
{
    // 表现组件不单独参与组件更新
    PrimaryComponentTick.bCanEverTick = false;
}

void UBBBMonsterPresentationComponent::ApplyPresentationState(
    const EBBBMonsterBehavior InState,
    const float InSpeed,
    const float InStateTime,
    const uint32 InActionId,
    const float InActionProgress)
{
    const bool bNewAction = !bHasAppliedAnimation || LastPlayedState != InState || LastActionId != InActionId || StateEnteredTime != InStateTime;

    // 保存 Mass 提供的只读快照 供蓝图或调试读取
    BBBMonsterBehavior = InState;
    MovementSpeed = FMath::Max(InSpeed, 0.0f);
    StateEnteredTime = InStateTime;

    // 移动相关状态循环播放 其余状态只播放一次
    const bool bLooping = InState == EBBBMonsterBehavior::Idle || InState == EBBBMonsterBehavior::Alert || InState == EBBBMonsterBehavior::Patrol || InState == EBBBMonsterBehavior::Chase;
    if (bNewAction)
    {
        USkeletalMeshComponent* const MonsterMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
        if (!ensureMsgf(MonsterMesh, TEXT("[UBBBM]Monster presentation requires a skeletal mesh component")))
        {
            return;
        }

        const bool bFactAnimation = Cast<UBBBMonsterFactAnimInstance>(MonsterMesh->GetAnimInstance()) != nullptr;
        if (!ensureMsgf(bFactAnimation, TEXT("[UBBBM]Monster presentation requires an authored monster animation blueprint")))
        {
            return;
        }

        USkeletalMeshComponentBudgeted* const BudgetMesh = Cast<USkeletalMeshComponentBudgeted>(MonsterMesh);
        if (BudgetMesh)
        {
            IAnimationBudgetAllocator* const Budget = IAnimationBudgetAllocator::Get(GetWorld());
            if (ensureMsgf(Budget, TEXT("[UBBBM]Budgeted action requires a world animation allocator")))
            {
                Budget->ForceNextTickThisFrame(BudgetMesh);
            }
        }
    }

    // 非循环动作由逻辑时间定位 禁用通知触发 避免不可见时漏伤或重复伤害
    if (!bLooping)
    {
        ActionProgress = FMath::Clamp(InActionProgress, 0.0f, 1.0f);
    }

    LastActionId = InActionId;
    LastPlayedState = InState;
    bHasAppliedAnimation = true;
}

EBBBMonsterBehavior UBBBMonsterPresentationComponent::GetBBBMonsterBehavior() const
{
    return BBBMonsterBehavior;
}

float UBBBMonsterPresentationComponent::GetMovementSpeed() const
{
    return MovementSpeed;
}

float UBBBMonsterPresentationComponent::GetStateEnteredTime() const
{
    return StateEnteredTime;
}
