#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationComponent.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"
#include "Components/SkeletalMeshComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterFactAnimInstance.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterHitReactionComponent.h"
#include "IAnimationBudgetAllocator.h"
#include "SkeletalMeshComponentBudgeted.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterVariationFragment.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysicsEngine/BodyInstance.h"
#include "PhysicsEngine/SkeletalBodySetup.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

bool UBBBMonsterPresentationComponent::BeginCorpsePresentation(const FVector& Velocity, const FBBBMonsterHitReactionFragment& Hit, const float Now, const bool bSimulate)
{
    if (bCorpseActive)
    {
        return true;
    }
    if (!bSimulate && CorpsePoseRequestFrame == MAX_uint64)
    {
        CorpsePoseRequestFrame = GFrameCounter;
        return false;
    }
    if (!bSimulate && CorpsePoseRequestFrame == GFrameCounter)
    {
        return false;
    }
    auto* Mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
    if (!ensureMsgf(Mesh && Mesh->GetPhysicsAsset() && Mesh->GetPhysicsAsset()->FindBodyIndex(TEXT("pelvis")) != INDEX_NONE,
        TEXT("[BBBCorpse]Death requires a pelvis physics body %s"), *GetOwner()->GetName()))
    {
        return false;
    }
    if (auto* Reaction = GetOwner()->FindComponentByClass<UBBBMonsterHitReactionComponent>())
    {
        Reaction->ResetPresentation();
        Reaction->ApplyHitFacts(Hit, false, false);
        Reaction->SetComponentTickEnabled(false);
    }
    LivingMeshRelativeTransform = Mesh->GetRelativeTransform();
    Mesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    Mesh->SetCanEverAffectNavigation(false);
    Mesh->SetGenerateOverlapEvents(false);
    Mesh->SetCollisionObjectType(ECC_PhysicsBody);
    Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    Mesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    Mesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
    Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
    if (auto* BudgetMesh = Cast<USkeletalMeshComponentBudgeted>(Mesh))
    {
        if (auto* Budget = IAnimationBudgetAllocator::Get(GetWorld()))
        {
            Budget->UnregisterComponent(BudgetMesh);
        }
    }
    if (!bSimulate)
    {
        const EVisibilityBasedAnimTickOption LivingTickOption = Mesh->VisibilityBasedAnimTickOption;
        Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
        Mesh->bEnableUpdateRateOptimizations = false;
        Mesh->bPauseAnims = false;
        Mesh->TickAnimation(0.35f, false);
        Mesh->RefreshBoneTransforms();
        Mesh->VisibilityBasedAnimTickOption = LivingTickOption;
        Mesh->bPauseAnims = true;
        Mesh->SetComponentTickEnabled(false);
        Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        bCorpseActive = true;
        bCorpseFrozen = true;
        CorpseStartedAt = Now;
        return true;
    }
    Mesh->bPauseAnims = true;
    Mesh->SetComponentTickEnabled(true);
    Mesh->SetAllBodiesSimulatePhysics(true);
    Mesh->SetAllBodiesPhysicsBlendWeight(1.0f);
    Mesh->SetEnableGravity(true);
    for (FBodyInstance* Body : Mesh->Bodies)
    {
        if (Body)
        {
            Body->SetEnableGravity(true);
            Body->LinearDamping = 0.6f;
            Body->AngularDamping = 0.8f;
            Body->UpdateDampingProperties();
        }
    }
    Mesh->SetAllPhysicsLinearVelocity(Velocity.ContainsNaN() ? FVector::ZeroVector : Velocity.GetClampedToMaxSize(650.0f));
    FVector CollapseImpulse = GetOwner()->GetActorForwardVector() * 100.0f;
    if (Hit.Serial != 0 && Hit.Age <= 0.3f && !Hit.Direction.ContainsNaN() && !Hit.Position.ContainsNaN())
    {
        const FVector Direction = Hit.Direction.GetSafeNormal();
        CollapseImpulse = Direction * 180.0f + FVector::UpVector * 25.0f;
    }
    Mesh->AddImpulse(CollapseImpulse, TEXT("pelvis"), true);
    Mesh->WakeAllRigidBodies();
    bCorpseActive = true;
    bCorpseFrozen = false;
    CorpseStartedAt = Now;
    CorpseStableSince = -1.0f;
    UE_LOG(LogTemp, Verbose, TEXT("[BBBCorpse]Begin Actor=%s Time=%.3f"), *GetOwner()->GetName(), Now);
    return true;
}

void UBBBMonsterPresentationComponent::UpdateCorpsePresentation(const float Now, const float MaximumDuration)
{
    if (!IsCorpseSimulating())
    {
        return;
    }
    auto* Mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
    if (!Mesh)
    {
        return;
    }
    float MaximumSpeedSquared = 0.0f;
    for (const FBodyInstance* Body : Mesh->Bodies)
    {
        if (Body && Body->IsInstanceSimulatingPhysics())
        {
            MaximumSpeedSquared = FMath::Max(MaximumSpeedSquared, static_cast<float>(Body->GetUnrealWorldVelocity().SizeSquared()));
        }
    }
    if (MaximumSpeedSquared > FMath::Square(15.0f))
    {
        CorpseStableSince = -1.0f;
    }
    if (MaximumSpeedSquared <= FMath::Square(15.0f) && CorpseStableSince < 0.0f)
    {
        CorpseStableSince = Now;
    }
    const bool bSettled = CorpseStableSince >= 0.0f && Now - CorpseStableSince >= 0.5f && Now - CorpseStartedAt >= 0.75f;
    FHitResult Floor;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBCorpseFloor), false, GetOwner());
    const FVector Pelvis = Mesh->GetSocketLocation(TEXT("pelvis"));
    const bool bNearFloor = GetWorld()->LineTraceSingleByChannel(Floor, Pelvis + FVector::UpVector * 5.0f,
        Pelvis - FVector::UpVector * 100.0f, ECC_WorldStatic, Params);
    if (bNearFloor && (bSettled || Now - CorpseStartedAt >= MaximumDuration))
    {
        FreezeCorpsePresentation();
    }
}

void UBBBMonsterPresentationComponent::FreezeCorpsePresentation()
{
    if (!IsCorpseSimulating())
    {
        return;
    }
    if (auto* Mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>())
    {
        Mesh->SetComponentTickEnabled(false);
        Mesh->PutAllRigidBodiesToSleep();
        Mesh->SetAllBodiesSimulatePhysics(false);
        Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Mesh->MarkRenderDynamicDataDirty();
    }
    bCorpseFrozen = true;
    UE_LOG(LogTemp, Verbose, TEXT("[BBBCorpse]Frozen Actor=%s"), *GetOwner()->GetName());
}

void UBBBMonsterPresentationComponent::ResetCorpsePresentation()
{
    CorpsePoseRequestFrame = MAX_uint64;
    if (!bCorpseActive)
    {
        return;
    }
    bHasAppliedAnimation = false;
    if (auto* Mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>())
    {
        Mesh->SetAllBodiesSimulatePhysics(false);
        Mesh->SetAllBodiesPhysicsBlendWeight(0.0f);
        for (FBodyInstance* Body : Mesh->Bodies)
        {
            auto* Setup = Body ? Cast<USkeletalBodySetup>(Body->GetBodySetup()) : nullptr;
            if (Setup)
            {
                Body->SetEnableGravity(Setup->DefaultInstance.bEnableGravity);
                Body->LinearDamping = Setup->DefaultInstance.LinearDamping;
                Body->AngularDamping = Setup->DefaultInstance.AngularDamping;
                Body->UpdateDampingProperties();
            }
        }
        Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Mesh->SetCollisionObjectType(ECC_Pawn);
        Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
        Mesh->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
        Mesh->SetRelativeTransform(LivingMeshRelativeTransform);
        Mesh->bPauseAnims = false;
        Mesh->SetComponentTickEnabled(true);
        Mesh->InitAnim(true);
        if (auto* BudgetMesh = Cast<USkeletalMeshComponentBudgeted>(Mesh))
        {
            if (auto* Budget = IAnimationBudgetAllocator::Get(GetWorld()))
            {
                Budget->RegisterComponent(BudgetMesh);
            }
        }
    }
    bCorpseActive = false;
    bCorpseFrozen = false;
    CorpseStartedAt = -1.0f;
    CorpseStableSince = -1.0f;
}

void UBBBMonsterPresentationComponent::ApplyVariationFacts(const FBBBMonsterVariationFragment& Variation, const float InLoopPhase)
{
    LoopPhase = InLoopPhase;
    if (VariationSeed == Variation.Seed)
    {
        return;
    }
    VariationSeed = Variation.Seed;
    LocomotionStyle = Variation.LocomotionStyle;
    AnimationSpeedScale = Variation.SpeedScale;
    if (auto* Mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>())
    {
        Mesh->SetCustomPrimitiveDataFloat(0, static_cast<float>(Variation.Infection) / 255.0f);
    }
}

void UBBBMonsterPresentationComponent::ApplyHitReaction(const FBBBMonsterHitReactionFragment& Hit)
{
    HitReaction = Hit;
    if (auto* Reaction = GetOwner()->FindComponentByClass<UBBBMonsterHitReactionComponent>())
    {
        Reaction->ApplyHitFacts(Hit, BBBMonsterBehavior != EBBBMonsterBehavior::Dead,
            bStaggering && BBBMonsterBehavior != EBBBMonsterBehavior::Attack && BBBMonsterBehavior != EBBBMonsterBehavior::Dead && !bCrawling);
    }
}

void UBBBMonsterPresentationComponent::ApplyMobilityState(const bool bInCrawling, const float InProgress, const float InHalfHeight)
{
    if (bCorpseActive)
    {
        return;
    }
    bCrawling = bInCrawling;
    CrawlProgress = FMath::Clamp(InProgress, 0.0f, 1.0f);
    if (auto* Mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>())
    {
        FVector Offset = Mesh->GetRelativeLocation();
        Offset.Z = -InHalfHeight;
        Mesh->SetRelativeLocation(Offset);
    }
}

void UBBBMonsterPresentationComponent::ApplyStaggerState(const bool bActive, const float Progress, const EBBBMonsterHitRegion Region)
{
    const bool bChanged = bStaggering != bActive;
    bStaggering = bActive;
    StaggerProgress = FMath::Clamp(Progress, 0.0f, 1.0f);
    StaggerRegion = Region;
    if (bChanged)
    {
        auto* Mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponentBudgeted>();
        auto* Budget = Mesh ? IAnimationBudgetAllocator::Get(GetWorld()) : nullptr;
        if (Budget)
        {
            Budget->ForceNextTickThisFrame(Mesh);
        }
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
