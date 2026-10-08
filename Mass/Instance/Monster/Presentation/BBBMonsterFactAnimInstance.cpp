#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterFactAnimInstance.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationComponent.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

void UBBBMonsterFactAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    AActor* const Owner = GetOwningActor();
    PresentationIdFact = Owner ? Owner->GetUniqueID() : 0;
    Presentation = Owner ? Owner->FindComponentByClass<UBBBMonsterPresentationComponent>() : nullptr;
    UWorld* const World = GetWorld();
    if (World && World->IsGameWorld())
    {
        ensureMsgf(Presentation.IsValid(), TEXT("[UBBBM]Fact animation requires a monster presentation component"));
    }
    BehaviorFact = EBBBMonsterBehavior::Idle;
    StaggeringFact = false;
    StaggerProgressFact = 0.0f;
    StaggerVariantFact = 1;
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
    StaggeringFact = Source->bStaggering && !CrawlingFact && BehaviorFact != EBBBMonsterBehavior::Attack && BehaviorFact != EBBBMonsterBehavior::Dead;
    StaggerProgressFact = Source->StaggerProgress;
    StaggerVariantFact = Source->StaggerRegion == EBBBMonsterHitRegion::Head ? 0
        : Source->StaggerRegion == EBBBMonsterHitRegion::RightArm || Source->StaggerRegion == EBBBMonsterHitRegion::RightLeg ? 2 : 1;
    CrawlProgressFact = Source->CrawlProgress;
    MovementSpeedFact = Source->MovementSpeed;
    ActionProgressFact = Source->ActionProgress;
    ActionIdFact = Source->LastActionId;
    StateEnteredTimeFact = Source->StateEnteredTime;
    HitRegionFact = static_cast<int32>(Source->HitReaction.Region);
    HitAgeFact = Source->HitReaction.Age;
    HitDirectionFact = GetSkelMeshComponent()->GetComponentTransform().InverseTransformVectorNoScale(Source->HitReaction.Direction);
}
