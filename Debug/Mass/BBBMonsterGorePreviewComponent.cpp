#include "BBBMonsterGorePreviewComponent.h"

#include "BBBZombieHitReactionTarget.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterGoreDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterGorePresentationComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterHitReactionComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterBloodPresentationSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterGibPresentationSubsystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

UBBBMonsterGorePreviewComponent::UBBBMonsterGorePreviewComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UBBBMonsterGorePreviewComponent::BeginPlay()
{
    Super::BeginPlay();
    if (auto* Target = Cast<ABBBZombieHitReactionTarget>(GetOwner()))
    {
        Target->ApplyConfiguration();
    }
}

void UBBBMonsterGorePreviewComponent::ApplyHit(const FBBBMonsterHitReactionFragment &Hit, const float Strength)
{
    if (!Definition || !Definition->IsReady() || !GetWorld())
    {
        return;
    }
    const int32 Region = static_cast<int32>(Hit.Region);
    if (Region < 0 || Region >= 6)
    {
        return;
    }
    CurrentHit = Hit;
    if (bAllowSevering && Region > 0 && ++RegionHits[Region] >= FMath::Max(1, HitsToSever))
    {
        Destroyed |= 1 << Region;
    }
    const double Now = GetWorld()->GetTimeSeconds();
    if (Definition->Blood && FMath::IsFinite(Strength) && Strength > 0 && Now >= NextBlood)
    {
        NextBlood = Now + 1 / FMath::Clamp(Definition->WoundUpdatesPerSecond, 1.0f, 30.0f);
        FBBBMonsterBloodImpact Impact;
        Impact.Position = Hit.Position;
        Impact.Direction = Hit.Direction;
        Impact.Normal = Hit.Normal;
        Impact.Region = Hit.Region;
        Impact.Seed = Hit.Serial ^ 17;
        Impact.Source = GetOwner();
        if (auto *Blood = GetWorld()->GetSubsystem<UBBBMonsterBloodPresentationSubsystem>())
        {
            Blood->Publish(*Definition->Blood, MakeArrayView(&Impact, 1));
        }
    }
    Present();
}

void UBBBMonsterGorePreviewComponent::Present()
{
    auto *Target = Cast<ABBBZombieHitReactionTarget>(GetOwner());
    if (Target && Target->bConfigurationReady && GetWorld()->IsGameWorld() && Definition && Definition->IsReady())
    {
        UBBBMonsterGorePresentationComponent::UpdateWounds(Result, *Definition, Destroyed, CurrentHit, Target->TargetMesh,
                                                      GetWorld()->GetTimeSeconds(), true);
        Target->Gore->ResidueSource = Target;
        Target->Gore->Apply(*Definition, Result, CurrentHit, FVector::ZeroVector, 17, false);
    }
}

void UBBBMonsterGorePreviewComponent::ResetTarget()
{
    auto *Target = Cast<ABBBZombieHitReactionTarget>(GetOwner());
    if (!Target || !GetWorld())
    {
        return;
    }
    Target->HitReaction->ResetPresentation();
    Target->Gore->ResetPresentation();
    Result = FBBBMonsterGoreFragment();
    CurrentHit = FBBBMonsterHitReactionFragment();
    FMemory::Memzero(RegionHits);
    Destroyed = 0;
    NextBlood = 0;
    NextReset = GetWorld()->GetTimeSeconds() + FMath::Max(0.0f, ResetInterval);
    AppliedInterval = ResetInterval;
    if (bClearResidueOnReset)
    {
        ClearResidue();
    }
    Present();
}

void UBBBMonsterGorePreviewComponent::TickComponent(const float DeltaTime, const ELevelTick TickType,
                                                    FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (!GetWorld() || !GetWorld()->IsGameWorld())
    {
        return;
    }
    auto* Target = Cast<ABBBZombieHitReactionTarget>(GetOwner());
    if (!Target || !Target->bConfigurationReady)
    {
        return;
    }
    Target->UpdatePreviewAnimation();
    if (AppliedInterval != ResetInterval)
    {
        AppliedInterval = ResetInterval;
        NextReset = GetWorld()->GetTimeSeconds() + FMath::Max(0.0f, ResetInterval);
    }
    if (ResetInterval > 0 && GetWorld()->GetTimeSeconds() >= NextReset)
    {
        ResetTarget();
    }
    CurrentHit.Age += DeltaTime;
    Present();
    if (auto *Blood = GetWorld()->GetSubsystem<UBBBMonsterBloodPresentationSubsystem>())
    {
        Blood->AdvancePresentation(DeltaTime);
    }
    if (auto *Pieces = GetWorld()->GetSubsystem<UBBBMonsterGibPresentationSubsystem>())
    {
        Pieces->AdvancePresentation(GetWorld()->GetTimeSeconds());
    }
}

void UBBBMonsterGorePreviewComponent::ClearResidue()
{
    if (GetWorld())
    {
        if (auto *Blood = GetWorld()->GetSubsystem<UBBBMonsterBloodPresentationSubsystem>())
        {
            Blood->ClearForSource(*GetOwner());
        }
    }
}

void UBBBMonsterGorePreviewComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ClearResidue();
    Super::EndPlay(EndPlayReason);
}
