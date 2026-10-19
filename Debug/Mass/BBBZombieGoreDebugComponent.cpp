#include "BBBZombieGoreDebugComponent.h"

#include "BBBZombieHitReactionTarget.h"
#include "BBBZombieSurfaceWoundComponent.h"
#include "BBBZombieGoreSurfaceDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterBloodPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterBloodPresentationSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterHitReactionComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

UBBBZombieGoreDebugComponent::UBBBZombieGoreDebugComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;
    static ConstructorHelpers::FObjectFinder<UBBBZombieGoreSurfaceDefinition> Appearance(TEXT(
        "/Game/_Project/Debug/ZombieHitReaction/Gore/DA_TargetGoreSurface.DA_TargetGoreSurface"));
    static ConstructorHelpers::FObjectFinder<UBBBMonsterBloodPresentationDefinition> Blood(TEXT(
        "/Game/_Project/Debug/ZombieHitReaction/DA_TargetBloodPresentation.DA_TargetBloodPresentation"));
    AppearanceDefinition = Appearance.Object;
    BloodSettings = Blood.Object;
    DamageRatios.Init(0, 6);
}

void UBBBZombieGoreDebugComponent::BeginPlay()
{
    Super::BeginPlay();
    auto* Target = CastChecked<ABBBZombieHitReactionTarget>(GetOwner());
    bSurfaceReady = AppearanceDefinition && BloodSettings
        && AppearanceDefinition->GetPathName().StartsWith(TEXT("/Game/_Project/Debug/"))
        && BloodSettings->GetPathName().StartsWith(TEXT("/Game/_Project/Debug/"))
        && Target->GorePresentation->InitializeSurface(*AppearanceDefinition);
    if (!bSurfaceReady)
    {
        UE_LOG(LogTemp, Error, TEXT("[BBB血腥调试]独立伤口断点资源不完整"));
        SetComponentTickEnabled(false);
        return;
    }
    ResetTarget();
    ResetCount = 0;
}

void UBBBZombieGoreDebugComponent::ApplyHit(const FBBBMonsterHitReactionFragment& Hit, const float WoundStrength)
{
    if (!bSurfaceReady || !GetWorld() || !GetWorld()->IsGameWorld())
    {
        return;
    }
    CurrentHit = Hit;
    const int32 Region = static_cast<int32>(Hit.Region);
    if (!DamageRatios.IsValidIndex(Region))
    {
        return;
    }
    auto* Surface = CastChecked<ABBBZombieHitReactionTarget>(GetOwner())->GorePresentation.Get();
    Surface->Stamp(Hit, Surface->DefaultStrength * WoundStrength);
    DamageRatios[Region] = FMath::Clamp(DamageRatios[Region] + DamageStep, 0.0f, 1.0f);
    if (bEnableHitBlood && BloodSettings)
    {
        FBBBMonsterBloodImpact Impact;
        Impact.Position = Hit.Position;
        Impact.Direction = Hit.Direction;
        Impact.Normal = Hit.Normal;
        Impact.Region = Hit.Region;
        Impact.Seed = Hit.Serial ^ static_cast<uint32>(PatternSeed);
        Impact.Source = GetOwner();
        Impact.bSevering = Region > 0 && bAllowSevering && DamageRatios[Region] >= 1
            && Surface->SelectedTiers[Region] == 0;
        GetWorld()->GetSubsystem<UBBBMonsterBloodPresentationSubsystem>()->Publish(*BloodSettings, MakeArrayView(&Impact, 1));
    }
    if (Region > 0 && bAllowSevering && DamageRatios[Region] >= 1)
    {
        Surface->Sever(Hit.Region, Hit);
    }
    ApplyCurrentWounds();
}

void UBBBZombieGoreDebugComponent::ApplyCurrentWounds()
{
    if (!bSurfaceReady)
    {
        return;
    }
    auto* Surface = CastChecked<ABBBZombieHitReactionTarget>(GetOwner())->GorePresentation.Get();
    Surface->SetStyle(BloodTint, TissueTint, bShowBloodStains ? BloodStrength : 0,
        TissueRoughness, TissueSpecular, bShowWounds);
}

void UBBBZombieGoreDebugComponent::ResetTarget()
{
    auto* Target = Cast<ABBBZombieHitReactionTarget>(GetOwner());
    if (!Target || !GetWorld() || !GetWorld()->IsGameWorld())
    {
        return;
    }
    Target->HitReaction->ResetPresentation();
    Target->GorePresentation->ResetSurface();
    DamageRatios.Init(0, 6);
    CurrentHit = FBBBMonsterHitReactionFragment();
    if (bClearResidueOnReset)
    {
        ClearBloodResidue();
    }
    ++ResetCount;
    AppliedResetInterval = ResetInterval;
    NextResetTime = GetWorld()->GetTimeSeconds() + FMath::Max(0.0f, ResetInterval);
    ApplyCurrentWounds();
}

void UBBBZombieGoreDebugComponent::PreviewSevering()
{
    const auto* Target = Cast<ABBBZombieHitReactionTarget>(GetOwner());
    if (!Target || !bSurfaceReady || PreviewRegion == EBBBMonsterHitRegion::Torso)
    {
        return;
    }
    const FName Bones[] = {TEXT("spine_02"), TEXT("head"), TEXT("lowerarm_l"), TEXT("lowerarm_r"), TEXT("calf_l"), TEXT("calf_r")};
    CurrentHit.Region = PreviewRegion;
    CurrentHit.Position = Target->TargetMesh->GetSocketLocation(Bones[static_cast<int32>(PreviewRegion)]);
    CurrentHit.Direction = -Target->GetActorForwardVector();
    CurrentHit.Normal = Target->GetActorForwardVector();
    CurrentHit.Age = 0;
    ++CurrentHit.Serial;
    bAllowSevering = true;
    DamageRatios[static_cast<int32>(PreviewRegion)] = 1;
    Target->GorePresentation->Sever(PreviewRegion, CurrentHit, PreviewCutTier);
}

void UBBBZombieGoreDebugComponent::ClearBloodResidue()
{
    if (GetWorld() && GetWorld()->IsGameWorld() && GetOwner())
    {
        GetWorld()->GetSubsystem<UBBBMonsterBloodPresentationSubsystem>()->ClearForSource(*GetOwner());
    }
}

void UBBBZombieGoreDebugComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (!bSurfaceReady || !GetWorld()->IsGameWorld())
    {
        return;
    }
    const double Now = GetWorld()->GetTimeSeconds();
    CurrentHit.Age = FMath::Min(CurrentHit.Age + DeltaTime, 10.0f);
    if (bAppliedAllowSevering != bAllowSevering)
    {
        ResetTarget();
        bAppliedAllowSevering = bAllowSevering;
    }
    if (AppliedResetInterval != ResetInterval)
    {
        AppliedResetInterval = ResetInterval;
        NextResetTime = Now + FMath::Max(0.0f, ResetInterval);
    }
    if (ResetInterval > 0 && Now >= NextResetTime)
    {
        ResetTarget();
    }
    ApplyCurrentWounds();
    CastChecked<ABBBZombieHitReactionTarget>(GetOwner())->GorePresentation->Advance(DeltaTime);
    GetWorld()->GetSubsystem<UBBBMonsterBloodPresentationSubsystem>()->AdvancePresentation(DeltaTime);
}

void UBBBZombieGoreDebugComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ClearBloodResidue();
    bSurfaceReady = false;
    Super::EndPlay(EndPlayReason);
}
