#include "BBBWork/UBBBNexus/Debug/Mass/BBBZombieGoreDebugComponent.h"

#include "BBBWork/UBBBNexus/Debug/Mass/BBBZombieHitReactionTarget.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterBloodPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterBloodPresentationSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterGibPresentationSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterHitReactionComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterSeveringPresentationComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

UBBBZombieGoreDebugComponent::UBBBZombieGoreDebugComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;
    static ConstructorHelpers::FObjectFinder<UBBBMonsterDefinition> Appearance(TEXT(
        "/Game/_Project/Debug/ZombieHitReaction/DA_TargetGoreAppearance.DA_TargetGoreAppearance"));
    static ConstructorHelpers::FObjectFinder<UBBBMonsterBloodPresentationDefinition> Blood(TEXT(
        "/Game/_Project/Debug/ZombieHitReaction/DA_TargetBloodPresentation.DA_TargetBloodPresentation"));
    AppearanceDefinition = Appearance.Object;
    BloodSettings = Blood.Object;
    DamageRatios.Init(0, 6);
}

void UBBBZombieGoreDebugComponent::BeginPlay()
{
    Super::BeginPlay();
    if (!AppearanceDefinition || !AppearanceDefinition->GetPathName().StartsWith(TEXT("/Game/_Project/Debug/"))
        || !BloodSettings || !BloodSettings->GetPathName().StartsWith(TEXT("/Game/_Project/Debug/")))
    {
        UE_LOG(LogTemp, Error, TEXT("[BBB血腥调试]需要独立调试外观配置"));
        SetComponentTickEnabled(false);
        return;
    }
    RuntimeDefinition = DuplicateObject<UBBBMonsterDefinition>(AppearanceDefinition, this);
    RuntimeDefinition->ClearFlags(RF_Public | RF_Standalone);
    RuntimeDefinition->SetFlags(RF_Transient);
    RuntimeDefinition->BloodPresentation = BloodSettings;
    RuntimeDefinition->SoundPresentation = nullptr;
    CastChecked<ABBBZombieHitReactionTarget>(GetOwner())->GorePresentation->ResidueSource = GetOwner();
    ResetTarget();
    ResetCount = 0;
}

void UBBBZombieGoreDebugComponent::ApplyHit(const FBBBMonsterHitReactionFragment& Hit)
{
    if (!RuntimeDefinition || !GetWorld() || !GetWorld()->IsGameWorld())
    {
        return;
    }
    CurrentHit = Hit;
    const int32 Region = static_cast<int32>(Hit.Region);
    if (!DamageRatios.IsValidIndex(Region))
    {
        return;
    }
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
        GetWorld()->GetSubsystem<UBBBMonsterBloodPresentationSubsystem>()->Publish(*BloodSettings, MakeArrayView(&Impact, 1));
    }
    ApplyCurrentWounds();
}

void UBBBZombieGoreDebugComponent::ApplyCurrentWounds()
{
    auto* Target = Cast<ABBBZombieHitReactionTarget>(GetOwner());
    if (!RuntimeDefinition || !Target || DamageRatios.Num() != 6)
    {
        return;
    }
    RuntimeDefinition->WoundStageThresholds = WoundThresholds;
    RuntimeDefinition->BloodPresentation = BloodSettings;
    FBBBMonsterHealthFragment Health;
    for (int32 Index = 0; Index < 6; ++Index)
    {
        float Ratio = FMath::Clamp(DamageRatios[Index], 0.0f, 1.0f);
        if (Index > 0 && !bAllowSevering)
        {
            Ratio = FMath::Min(Ratio, 0.99f);
        }
        Health.PartDamageRatios.Add(static_cast<EBBBMonsterHitRegion>(Index), Ratio);
        if (Index > 0 && bAllowSevering && Ratio >= 1)
        {
            Health.DestroyedParts |= 1u << Index;
        }
    }
    Target->GorePresentation->ApplyWoundFacts(Health, *RuntimeDefinition, CurrentHit, FVector::ZeroVector,
        static_cast<uint32>(FMath::Max(0, PatternSeed)), false);

    // 这些额外槽位仅由独立调试材质读取 不给正式尸群增加动态材质实例
    const float Values[] = {BloodTint.R, BloodTint.G, BloodTint.B,
        bShowBloodStains ? FMath::Clamp(BloodStrength, 0.0f, 3.0f) : 0.0f,
        TissueTint.R, TissueTint.G, TissueTint.B, FMath::Clamp(WoundScale, 0.1f, 2.0f),
        FMath::Clamp(EdgeScale, 0.0f, 3.0f), FMath::Clamp(TissueRoughness, 0.0f, 1.0f),
        FMath::Clamp(TissueSpecular, 0.0f, 1.0f), bShowWounds ? 1.0f : 0.0f};
    TInlineComponentArray<USkeletalMeshComponent*> Meshes;
    Target->GetComponents(Meshes);
    for (auto* Mesh : Meshes)
    {
        for (int32 Index = 0; Index < UE_ARRAY_COUNT(Values); ++Index)
        {
            Mesh->SetCustomPrimitiveDataFloat(10 + Index, Values[Index]);
        }
    }
}

void UBBBZombieGoreDebugComponent::ResetTarget()
{
    auto* Target = Cast<ABBBZombieHitReactionTarget>(GetOwner());
    if (!Target || !GetWorld() || !GetWorld()->IsGameWorld())
    {
        return;
    }
    Target->GorePresentation->ResetPresentation();
    Target->HitReaction->ResetPresentation();
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

void UBBBZombieGoreDebugComponent::ApplyPreview()
{
    if (!GetWorld() || !GetWorld()->IsGameWorld() || DamageRatios.Num() != 6)
    {
        return;
    }
    DamageRatios[FMath::Clamp(static_cast<int32>(PreviewRegion), 0, 5)] = FMath::Clamp(PreviewDamageRatio, 0.0f, 1.0f);
    CastChecked<ABBBZombieHitReactionTarget>(GetOwner())->GorePresentation->ResetPresentation();
    CurrentHit.Age = 10;
    ApplyCurrentWounds();
}

void UBBBZombieGoreDebugComponent::PreviewSevering()
{
    const auto* Target = Cast<ABBBZombieHitReactionTarget>(GetOwner());
    if (!Target || !RuntimeDefinition || PreviewRegion == EBBBMonsterHitRegion::Torso)
    {
        return;
    }
    const FName Bones[] = {TEXT("spine_02"), TEXT("head"), TEXT("upperarm_l"), TEXT("upperarm_r"), TEXT("thigh_l"), TEXT("thigh_r")};
    bAllowSevering = true;
    CurrentHit.Region = PreviewRegion;
    CurrentHit.Position = Target->TargetMesh->GetSocketLocation(Bones[FMath::Clamp(static_cast<int32>(PreviewRegion), 0, 5)]);
    CurrentHit.Direction = -Target->GetActorForwardVector();
    CurrentHit.Normal = Target->GetActorForwardVector();
    CurrentHit.Age = 0;
    ++CurrentHit.Serial;
    DamageRatios[FMath::Clamp(static_cast<int32>(PreviewRegion), 0, 5)] = 1;
    ApplyCurrentWounds();
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
    if (!RuntimeDefinition || !GetWorld()->IsGameWorld())
    {
        return;
    }
    const double Now = GetWorld()->GetTimeSeconds();
    CurrentHit.Age = FMath::Min(CurrentHit.Age + DeltaTime, 10.0f);
    if (bAppliedAllowSevering != bAllowSevering)
    {
        CastChecked<ABBBZombieHitReactionTarget>(GetOwner())->GorePresentation->ResetPresentation();
        bAppliedAllowSevering = bAllowSevering;
        CurrentHit.Age = 10;
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
    GetWorld()->GetSubsystem<UBBBMonsterBloodPresentationSubsystem>()->AdvancePresentation(DeltaTime);
    GetWorld()->GetSubsystem<UBBBMonsterGibPresentationSubsystem>()->AdvancePresentation(static_cast<float>(Now));
}

void UBBBZombieGoreDebugComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ClearBloodResidue();
    RuntimeDefinition = nullptr;
    Super::EndPlay(EndPlayReason);
}
