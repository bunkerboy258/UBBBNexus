#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterHitReactionComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "HitReactProfile.h"
#include "HitReactStatics.h"
#include "IAnimationBudgetAllocator.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "SkeletalMeshComponentBudgeted.h"

UBBBMonsterHitReactionComponent::UBBBMonsterHitReactionComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bUseFixedSimulationRate = false;
    Cooldown = 0.0f;
    const TCHAR* ProfileNames[] = {TEXT("HRP_BBBZombieTorso"), TEXT("HRP_BBBZombieHead"), TEXT("HRP_BBBZombieArms"), TEXT("HRP_BBBZombieLegs")};
    for (const TCHAR* Name : ProfileNames)
    {
        AvailableProfiles.Add(TSoftObjectPtr<UHitReactProfile>(FSoftObjectPath(FString::Printf(
            TEXT("/Game/_Project/System/Mass/Monster/Zombie/Shared/HitReaction/%s.%s"), Name, Name))));
    }
}

void UBBBMonsterHitReactionComponent::ApplyHitFacts(const FBBBMonsterHitReactionFragment& Hit, const bool bAlive)
{
    bPresentationAlive = bAlive;
    if (!bAlive)
    {
        ResetHitReactSystem();
        ObservedHitSerial = Hit.Serial;
        return;
    }

    if (Hit.Serial == 0 || Hit.Serial == ObservedHitSerial)
    {
        return;
    }

    if (!bHasInitialized || !bProfilesLoaded)
    {
        return;
    }

    if (!CanHitReact() || Hit.Age > 0.2f || Hit.Direction.ContainsNaN() || Hit.Direction.IsNearlyZero() || Hit.Position.ContainsNaN())
    {
        ObservedHitSerial = Hit.Serial;
        return;
    }

    ObservedHitSerial = Hit.Serial;
    const UPhysicsAsset* PhysicsAsset = Mesh->GetPhysicsAsset();
    if (!ensureMsgf(PhysicsAsset, TEXT("[BBBHitReact]表现网格缺少物理资产 %s"), *GetOwner()->GetName()))
    {
        return;
    }

    int32 ProfileIndex = 0;
    FName BoneName = TEXT("spine_03");
    float LinearStrength = 350.0f;
    float AngularStrength = 800.0f;
    switch (Hit.Region)
    {
        case EBBBMonsterHitRegion::Head:
            ProfileIndex = 1;
            BoneName = TEXT("head");
            LinearStrength = 260.0f;
            AngularStrength = 1200.0f;
            break;
        case EBBBMonsterHitRegion::LeftArm:
        case EBBBMonsterHitRegion::RightArm:
            ProfileIndex = 2;
            BoneName = Hit.Region == EBBBMonsterHitRegion::LeftArm ? TEXT("upperarm_l") : TEXT("upperarm_r");
            if (PhysicsAsset->FindBodyIndex(BoneName) == INDEX_NONE)
            {
                BoneName = Hit.Region == EBBBMonsterHitRegion::LeftArm ? TEXT("clavicle_l") : TEXT("clavicle_r");
            }
            LinearStrength = 500.0f;
            AngularStrength = 2000.0f;
            break;
        case EBBBMonsterHitRegion::LeftLeg:
        case EBBBMonsterHitRegion::RightLeg:
            ProfileIndex = 3;
            BoneName = Hit.Region == EBBBMonsterHitRegion::LeftLeg ? TEXT("thigh_l") : TEXT("thigh_r");
            LinearStrength = 250.0f;
            AngularStrength = 600.0f;
            break;
        default:
            break;
    }

    if (!ensureMsgf(PhysicsAsset->FindBodyIndex(BoneName) != INDEX_NONE && AvailableProfiles.IsValidIndex(ProfileIndex),
        TEXT("[BBBHitReact]受击部位缺少物理刚体或配置 %s %s"), *GetOwner()->GetName(), *BoneName.ToString()))
    {
        return;
    }

    const FVector Direction = Hit.Direction.GetSafeNormal();
    const FVector Lever = Hit.Position - Mesh->GetBoneLocation(BoneName);
    FVector AngularAxis = FVector::CrossProduct(Lever, Direction).GetSafeNormal();
    if (AngularAxis.IsNearlyZero())
    {
        AngularAxis = FVector::CrossProduct(FVector::UpVector, Direction).GetSafeNormal();
    }

    const float Now = GetWorld()->GetTimeSeconds();
    const float RepeatedScale = LastAppliedTime < 0.0f ? 1.0f : FMath::Lerp(0.55f, 1.0f, FMath::Clamp((Now - LastAppliedTime) / 0.25f, 0.0f, 1.0f));
    FHitReactImpulseParams Impulse;
    Impulse.LinearImpulse.bApplyImpulse = true;
    Impulse.LinearImpulse.Impulse = LinearStrength;
    Impulse.AngularImpulse.bApplyImpulse = !AngularAxis.IsNearlyZero();
    Impulse.AngularImpulse.Impulse = AngularStrength;
    FHitReactImpulse_WorldParams World;
    World.LinearDirection = Direction;
    World.AngularDirection = AngularAxis;
    const FHitReactInputParams Params(AvailableProfiles[ProfileIndex], BoneName, true);
    RequestAnimationUpdate();
    if (HitReact(Params, Impulse, World, RepeatedScale))
    {
        LastAppliedTime = Now;
        if (IsValid(PhysicalAnimation))
        {
            PhysicalAnimation->AddTickPrerequisiteComponent(this);
            PhysicalAnimation->SetComponentTickEnabled(true);
        }
        UE_LOG(LogTemp, Verbose, TEXT("[BBBHitReact]命中编号=%u 部位=%s 力度=%.2f"), Hit.Serial, *BoneName.ToString(), RepeatedScale);
    }
}

bool UBBBMonsterHitReactionComponent::CanHitReact_Implementation() const
{
    return bPresentationAlive && IsValid(GetOwner()) && !GetOwner()->IsHidden() && IsValid(Mesh)
        && Mesh->IsVisible() && Mesh->GetPredictedLODLevel() <= 1;
}

void UBBBMonsterHitReactionComponent::RequestAnimationUpdate() const
{
    auto* BudgetMesh = Cast<USkeletalMeshComponentBudgeted>(Mesh);
    auto* Budget = BudgetMesh && GetWorld() ? IAnimationBudgetAllocator::Get(GetWorld()) : nullptr;
    if (Budget)
    {
        Budget->ForceNextTickThisFrame(BudgetMesh);
    }
}

void UBBBMonsterHitReactionComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    if (!CanHitReact())
    {
        ResetHitReactSystem();
        return;
    }

    const bool bWasReacting = !PhysicsBlends.IsEmpty();
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (!PhysicsBlends.IsEmpty())
    {
        RequestAnimationUpdate();
    }
    if (bWasReacting && PhysicsBlends.IsEmpty())
    {
        ResetHitReactSystem();
    }
}

void UBBBMonsterHitReactionComponent::ResetHitReactSystem()
{
    if (IsValid(PhysicalAnimation))
    {
        PhysicalAnimation->SetComponentTickEnabled(false);
    }
    const bool bHadReaction = LastAppliedTime >= 0.0f || !PhysicsBlends.IsEmpty() || PendingImpulse.IsValid() || bCollisionEnabledChanged || bPhysicalAnimationProfileChanged || bConstraintProfileChanged;
    if (bHadReaction && IsValid(Mesh))
    {
        Mesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
        Mesh->SetAllPhysicsAngularVelocityInRadians(FVector::ZeroVector);
        Mesh->SetAllBodiesPhysicsBlendWeight(0.0f);
        Mesh->SetAllBodiesSimulatePhysics(false);
        Mesh->SetConstraintProfileForAll(NAME_None);
        if (IsValid(PhysicalAnimation))
        {
            PhysicalAnimation->SetSkeletalMeshComponent(nullptr);
            PhysicalAnimation->SetSkeletalMeshComponent(Mesh);
        }
        if (bCollisionEnabledChanged)
        {
            Mesh->SetCollisionEnabled(DefaultCollisionEnabled);
        }
        UHitReactStatics::FinalizeMeshPhysics(Mesh);
    }

    PhysicsBlends.Reset();
    SmoothedBoneWeights.Reset();
    PendingImpulse = {};
    LastProfileHitReactTimes.Reset();
    LastHitReactTime = -1.0f;
    LastAppliedTime = -1.0f;
    bCollisionEnabledChanged = false;
    bPhysicalAnimationProfileChanged = false;
    bConstraintProfileChanged = false;
    SleepHitReact();
}

void UBBBMonsterHitReactionComponent::ResetPresentation()
{
    ResetHitReactSystem();
    ObservedHitSerial = 0;
    bPresentationAlive = true;
}

uint32 UBBBMonsterHitReactionComponent::GetObservedHitSerial() const
{
    return ObservedHitSerial;
}

void UBBBMonsterHitReactionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ResetHitReactSystem();
    CancelAsyncLoading();
    Super::EndPlay(EndPlayReason);
}
