#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterFactAnimInstance.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterPresentationDefinition.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationComponent.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

void UBBBMonsterFactAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    AActor* const Owner = GetOwningActor();
    Presentation = Owner ? Owner->FindComponentByClass<UBBBMonsterPresentationComponent>() : nullptr;
    UWorld* const World = GetWorld();
    if (World && World->IsGameWorld())
    {
        ensureMsgf(Presentation.IsValid(), TEXT("[UBBBM]Fact animation requires a monster presentation component"));
    }
    TraversalActiveFact = false;
    TraversalHandAlphaFact = 0;
    MantleSequenceFact = nullptr;
    ClimbSequenceFact = nullptr;
    BehaviorFact = EBBBMonsterBehavior::Idle;
    StaggeringFact = false;
    StaggerProgressFact = 0.0f;
    StaggerVariantFact = 0;
    SpecialFact = {};
    SpecialPhaseFact = 0;
    SpecialProgressFact = 0;
    CrawlingFact = false;
    CrawlProgressFact = 0.0f;
    MovementSpeedFact = 0.0f;
    ActionProgressFact = 0.0f;
    ActionIdFact = 0;
    StateEnteredTimeFact = 0.0f;
    ActorYawFact = GetOwningActor() ? GetOwningActor()->GetActorRotation().Yaw : 0.0f;
}

void UBBBMonsterFactAnimInstance::NativeUpdateAnimation(const float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    const UBBBMonsterPresentationComponent* const Source = Presentation.Get();
    if (!Source || !Source->bHasAppliedAnimation)
    {
        return;
    }

    const auto* TraversalConfig = Source->TraversalSettings.Get();
    SpecialFact = Source->SpecialFact;
    SpecialPhaseFact = Source->BBBMonsterBehavior != EBBBMonsterBehavior::Dead && !Source->bCrawling
        && SpecialFact.HoldsMovement() ? SpecialFact.Phase : 0;
    SpecialProgressFact = FMath::Clamp((GetWorld()->GetTimeSeconds() - SpecialFact.StartedAt)
        / FMath::Max(SpecialFact.Duration, 0.01f), 0.0f, 1.0f);
    const auto& Traversal = Source->TraversalFact;
    TraversalActiveFact = TraversalConfig && Traversal.Kind != 0
        && Source->BBBMonsterBehavior != EBBBMonsterBehavior::Dead && !Source->bCrawling;
    TraversalHighFact = Traversal.Kind == 2;
    TraversalHandAlphaFact = 0;
    if (TraversalConfig)
    {
        MantleSequenceFact = TraversalConfig->Mantle.Sequence;
        ClimbSequenceFact = TraversalConfig->Climb.Sequence;
    }
    if (TraversalActiveFact)
    {
        const float P = FMath::Clamp((GetWorld()->GetTimeSeconds() - Traversal.StartedAt)
            / FMath::Max(Traversal.Duration, 0.01f), 0.0f, 1.0f);
        MantleTimeFact = MantleSequenceFact ? P * MantleSequenceFact->GetPlayLength() : 0;
        ClimbTimeFact = ClimbSequenceFact ? P * ClimbSequenceFact->GetPlayLength() : 0;
        const auto& Clip = TraversalHighFact ? TraversalConfig->Climb : TraversalConfig->Mantle;
        const float In = FMath::Clamp((P - Clip.ContactStart) / 0.08f, 0.0f, 1.0f);
        const float Out = FMath::Clamp((Clip.ContactEnd - P) / 0.08f, 0.0f, 1.0f);
        TraversalHandAlphaFact = FMath::Min(In, Out);
        const FVector Side = GetOwningActor()->GetActorRightVector();
        auto* Mesh = GetSkelMeshComponent();
        const FTransform Component = Mesh->GetComponentTransform();
        const float LeftSign = FVector::DotProduct(Mesh->GetSocketLocation(TEXT("upperarm_l"))
            - GetOwningActor()->GetActorLocation(), Side) < 0 ? -1.0f : 1.0f;
        TraversalLeftHandFact = Component.InverseTransformPosition(Traversal.Ledge + Side * Clip.HandSpacing * 0.5f * LeftSign);
        TraversalRightHandFact = Component.InverseTransformPosition(Traversal.Ledge - Side * Clip.HandSpacing * 0.5f * LeftSign);
        TraversalLeftElbowFact = Component.InverseTransformPosition(Mesh->GetSocketLocation(TEXT("lowerarm_l")) + Side * 25.0f * LeftSign);
        TraversalRightElbowFact = Component.InverseTransformPosition(Mesh->GetSocketLocation(TEXT("lowerarm_r")) - Side * 25.0f * LeftSign);
    }
    BehaviorFact = Source->BBBMonsterBehavior;
    ActorYawFact = GetOwningActor()->GetActorRotation().Yaw;
    CrawlingFact = Source->bCrawling;
    StaggeringFact = Source->bStaggering && !TraversalActiveFact && !CrawlingFact && BehaviorFact != EBBBMonsterBehavior::Dead;
    StaggerProgressFact = Source->StaggerProgress;
    StaggerVariantFact = 0;
    switch (Source->StaggerRegion)
    {
        case EBBBMonsterHitRegion::LeftArm:
            StaggerVariantFact = 1;
            break;
        case EBBBMonsterHitRegion::RightArm:
            StaggerVariantFact = 2;
            break;
        case EBBBMonsterHitRegion::LeftLeg:
            StaggerVariantFact = 3;
            break;
        case EBBBMonsterHitRegion::RightLeg:
            StaggerVariantFact = 4;
            break;
        default:
            break;
    }
    CrawlProgressFact = Source->CrawlProgress;
    MovementSpeedFact = Source->MovementSpeed;
    VariationSeedFact = Source->VariationSeed;
    LocomotionStyleFact = Source->LocomotionStyle;
    AnimationSpeedFact = Source->MovementSpeed / FMath::Max(Source->AnimationSpeedScale, 0.01f);
    LoopPhaseFact = Source->LoopPhase;
    ActionProgressFact = Source->ActionProgress;
    ActionIdFact = Source->LastActionId;
    StateEnteredTimeFact = Source->StateEnteredTime;
    HitRegionFact = static_cast<int32>(Source->HitReaction.Region);
    HitAgeFact = Source->HitReaction.Age;
    HitDirectionFact = GetSkelMeshComponent()->GetComponentTransform().InverseTransformVectorNoScale(Source->HitReaction.Direction);
}
