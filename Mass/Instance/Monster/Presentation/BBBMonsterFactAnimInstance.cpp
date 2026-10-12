#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterFactAnimInstance.h"

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
    BehaviorFact = EBBBMonsterBehavior::Idle;
    StaggeringFact = false;
    StaggerProgressFact = 0.0f;
    StaggerVariantFact = 0;
    CrawlingFact = false;
    CrawlProgressFact = 0.0f;
    MovementSpeedFact = 0.0f;
    ActionProgressFact = 0.0f;
    ActionIdFact = 0;
    StateEnteredTimeFact = 0.0f;
}

void UBBBMonsterFactAnimInstance::NativeUpdateAnimation(const float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    const UBBBMonsterPresentationComponent* const Source = Presentation.Get();
    if (!Source || !Source->bHasAppliedAnimation)
    {
        return;
    }

    BehaviorFact = Source->BBBMonsterBehavior;
    CrawlingFact = Source->bCrawling;
    StaggeringFact = Source->bStaggering && !CrawlingFact && BehaviorFact != EBBBMonsterBehavior::Dead;
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
