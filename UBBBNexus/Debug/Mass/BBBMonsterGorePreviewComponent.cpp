#include "BBBMonsterGorePreviewComponent.h"

#include "BBBZombieHitReactionTarget.h"
#include "BBBWork/UBBBNexus/Combat/FBBBHitContribution.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterDestructionSystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterDismembermentSystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDestructionContribution.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
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

void UBBBMonsterGorePreviewComponent::ApplyHit(const FBBBMonsterHitReactionFragment& Hit,
    const FBBBHitContribution& HitContribution, const UBBBMonsterDefinition& Rules)
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
    const auto BodyPart = static_cast<EBBBMonsterHitRegion>(Region);
    const auto& PartRule = Rules.GetBodyPart(BodyPart);
    if (!PartRule.IsValid() || !FMath::IsFinite(Rules.MaxHealth) || Rules.MaxHealth <= 0.0f)
    {
        return;
    }
    const double EffectiveDamage = PartRule.ResolveDamage(HitContribution.AddDamage,
        HitContribution.AddDurableDamage);
    if (!FMath::IsFinite(EffectiveDamage) || EffectiveDamage < 0.0)
    {
        return;
    }
    CurrentHit = Hit;
    auto* Target = Cast<ABBBZombieHitReactionTarget>(GetOwner());
    if (bAllowSevering && EffectiveDamage > 0.0)
    {
        PartDamage.Add(BodyPart, EffectiveDamage);
        if (Region > 0 && PartRule.HasReachedHealthThreshold(PartDamage.Get(BodyPart), Rules.MaxHealth))
        {
            Destroyed |= 1 << Region;
        }
    }
    if (Region == 0 && Target && Target->TargetMesh && HitContribution.AddWound > 0.0f && EffectiveDamage > 0.0)
    {
        FBBBMonsterDestructionContribution Destruction;
        FMemory::Memcpy(Destruction.Regions, Result.DestructionStrengths, sizeof(Destruction.Regions));
        const int32 Zone = FBBBMonsterDestructionSystem::Select(*Definition, Hit.Position, Hit.Direction,
            Target->TargetMesh->GetComponentTransform(), Target->TargetMesh);
        Destruction.Add(Zone, HitContribution.AddWound * Definition->Destruction->HitAccumulation);
        FMemory::Memcpy(Result.DestructionStrengths, Destruction.Regions, sizeof(Destruction.Regions));
    }
    const double Now = GetWorld()->GetTimeSeconds();
    if (Definition->GetBlood() && FMath::IsFinite(HitContribution.AddWound)
        && HitContribution.AddWound > 0.0f && Now >= NextBlood)
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
            Blood->Publish(*Definition->GetBlood(), MakeArrayView(&Impact, 1));
        }
    }
    Present();
}

void UBBBMonsterGorePreviewComponent::Present()
{
    auto *Target = Cast<ABBBZombieHitReactionTarget>(GetOwner());
    if (Target && Target->bConfigurationReady && GetWorld()->IsGameWorld() && Definition && Definition->IsReady())
    {
        FBBBMonsterDismembermentSystem::Resolve(Result, *Definition, Destroyed, CurrentHit, Target->TargetMesh);
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
    PartDamage = FBBBMonsterPartDamage();
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
    UWorld* World = GetWorld();
    AActor* Owner = GetOwner();
    if (!World || !Owner)
    {
        return;
    }
    if (auto* Blood = World->GetSubsystem<UBBBMonsterBloodPresentationSubsystem>())
    {
        Blood->ClearForSource(*Owner);
    }
}

void UBBBMonsterGorePreviewComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ClearResidue();
    Super::EndPlay(EndPlayReason);
}
