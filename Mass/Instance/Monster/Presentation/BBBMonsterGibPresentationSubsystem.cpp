#include "BBBMonsterGibPresentationSubsystem.h"

#include "Components/PoseableMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSoundPresentationDefinition.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

namespace
{
    constexpr int32 MaximumSimulating = 32;
    constexpr int32 MaximumSettled = 128;
    constexpr float Lifetime = 30.0f;

    float ViewDistanceSquared(const UWorld& World, const FVector& Location)
    {
        float Result = TNumericLimits<float>::Max();
        for (auto It = World.GetPlayerControllerIterator(); It; ++It)
        {
            FVector Eye;
            FRotator Rotation;
            if (const auto* Controller = It->Get(); Controller && Controller->IsLocalController())
            {
                Controller->GetPlayerViewPoint(Eye, Rotation);
                Result = FMath::Min(Result, static_cast<float>(FVector::DistSquared(Eye, Location)));
            }
        }
        return Result;
    }
}

int32 UBBBMonsterGibPresentationSubsystem::GetSimulatingCount() const
{
    int32 Result = 0;
    for (int32 Index = 0; Index < Pieces.Num(); ++Index)
    {
        Result += BornAt[Index] >= 0.0f && Pieces[Index]->IsSimulatingPhysics() ? 1 : 0;
    }
    return Result;
}

int32 UBBBMonsterGibPresentationSubsystem::GetSettledCount() const
{
    int32 Result = 0;
    for (int32 Index = 0; Index < Pieces.Num(); ++Index)
    {
        Result += BornAt[Index] >= 0.0f && !Pieces[Index]->IsSimulatingPhysics() ? 1 : 0;
    }
    return Result;
}

void UBBBMonsterGibPresentationSubsystem::ReleaseSlot(const int32 Index)
{
    Pieces[Index]->SetSimulatePhysics(false);
    Pieces[Index]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Pieces[Index]->SetVisibility(false);
    Poses[Index]->SetVisibility(false);
    BornAt[Index] = -1.0f;
    StableSince[Index] = -1.0f;
    LandingPlayed[Index] = false;
    SoundConfigs[Index] = nullptr;
}

int32 UBBBMonsterGibPresentationSubsystem::GetLandedCount() const
{
    int32 Result = 0;
    for (int32 Index = 0; Index < Pieces.Num(); ++Index)
    {
        Result += BornAt[Index] >= 0.0f && LandingPlayed[Index] ? 1 : 0;
    }
    return Result;
}

int32 UBBBMonsterGibPresentationSubsystem::FindSettledRecycleSlot() const
{
    int32 Result = INDEX_NONE;
    float BestScore = -1.0f;
    const UWorld* World = GetWorld();
    for (int32 Index = 0; World && Index < Pieces.Num(); ++Index)
    {
        if (BornAt[Index] < 0.0f || Pieces[Index]->IsSimulatingPhysics())
        {
            continue;
        }
        const bool bVisible = Pieces[Index]->WasRecentlyRendered(0.2f) || Poses[Index]->WasRecentlyRendered(0.2f);
        const float Score = ViewDistanceSquared(*World, Pieces[Index]->GetComponentLocation())
            + (bVisible ? 0.0f : 100000000.0f) + (World->GetTimeSeconds() - BornAt[Index]) * 1000.0f;
        if (Score > BestScore)
        {
            BestScore = Score;
            Result = Index;
        }
    }
    return Result;
}

bool UBBBMonsterGibPresentationSubsystem::Emit(USkeletalMeshComponent& Source, UStaticMesh& Part,
    USkeletalMesh& PosePart, const FName Bone, const FVector& Direction, const FVector& Velocity,
    const uint32 Seed, UBBBMonsterSoundPresentationDefinition* Sounds)
{
    UWorld* World = GetWorld();
    if (!World || Direction.ContainsNaN() || Velocity.ContainsNaN() || Part.IsCompiling()
        || World->GetNetMode() == NM_DedicatedServer || !Source.WasRecentlyRendered(0.2f)
        || Source.GetPredictedLODLevel() > 2 || Source.bHiddenInGame || (Source.GetOwner() && Source.GetOwner()->IsHidden()))
    {
        return false;
    }
    const FTransform BoneWorld = Source.GetSocketTransform(Bone);
    const float DistanceSquared = ViewDistanceSquared(*World, BoneWorld.GetLocation());
    if (DistanceSquared > FMath::Square(2500.0f) || GetSimulatingCount() >= MaximumSimulating)
    {
        return false;
    }
    int32 Slot = BornAt.IndexOfByPredicate([](const float Value) { return Value < 0.0f; });
    if (Slot == INDEX_NONE && Pieces.Num() >= MaximumSimulating + MaximumSettled)
    {
        Slot = FindSettledRecycleSlot();
        if (Slot == INDEX_NONE)
        {
            return false;
        }
        ReleaseSlot(Slot);
    }
    if (Slot == INDEX_NONE)
    {
        AActor* PoolActor = Pieces.IsEmpty() ? nullptr : Pieces[0]->GetOwner();
        if (!PoolActor)
        {
            FActorSpawnParameters Parameters;
            Parameters.ObjectFlags |= RF_Transient;
            PoolActor = World->SpawnActor<AActor>(Parameters);
            if (!PoolActor)
            {
                return false;
            }
            PoolActor->SetActorTickEnabled(false);
            PoolActor->SetReplicates(false);
        }
        /** 全池共用载体 UE 碰撞派发要求刚体具有所属 Actor */
        auto* Piece = NewObject<UStaticMeshComponent>(PoolActor);
        PoolActor->AddInstanceComponent(Piece);
        Piece->PrimaryComponentTick.bCanEverTick = false;
        Piece->SetCanEverAffectNavigation(false);
        Piece->SetCollisionObjectType(ECC_PhysicsBody);
        Piece->SetCollisionResponseToAllChannels(ECR_Ignore);
        Piece->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
        Piece->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
        Piece->SetLinearDamping(0.25f);
        Piece->SetAngularDamping(0.9f);
        Piece->SetGenerateOverlapEvents(false);
        Piece->SetNotifyRigidBodyCollision(true);
        Piece->OnComponentHit.AddUniqueDynamic(this, &UBBBMonsterGibPresentationSubsystem::OnPieceHit);
        Piece->RegisterComponentWithWorld(World);
        auto* Pose = NewObject<UPoseableMeshComponent>(PoolActor);
        PoolActor->AddInstanceComponent(Pose);
        Pose->PrimaryComponentTick.bCanEverTick = false;
        Pose->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Pose->SetCanEverAffectNavigation(false);
        Pose->SetupAttachment(Piece);
        Pose->RegisterComponentWithWorld(World);
        Slot = Pieces.Add(Piece);
        Poses.Add(Pose);
        BornAt.Add(-1.0f);
        StableSince.Add(-1.0f);
        SoundConfigs.Add(nullptr);
        LandingPlayed.Add(false);
    }
    if (!ContactMaterial)
    {
        ContactMaterial = NewObject<UPhysicalMaterial>(this);
        ContactMaterial->Friction = 0.65f;
        ContactMaterial->StaticFriction = 0.8f;
        ContactMaterial->Restitution = 0.12f;
    }
    auto* Piece = Pieces[Slot].Get();
    auto* Pose = Poses[Slot].Get();
    Piece->SetStaticMesh(&Part);
    Piece->SetPhysMaterialOverride(ContactMaterial);
    Piece->SetWorldTransform(BoneWorld, false, nullptr, ETeleportType::TeleportPhysics);
    const bool bCapturePose = DistanceSquared <= FMath::Square(1000.0f);
    Piece->SetVisibility(!bCapturePose);
    Pose->SetVisibility(bCapturePose);
    if (bCapturePose)
    {
        Pose->SetSkinnedAssetAndUpdate(&PosePart);
        Pose->SetRelativeTransform(Source.GetComponentTransform().GetRelativeTransform(BoneWorld));
        const auto& Captured = Source.GetComponentSpaceTransforms();
        const auto& Skeleton = PosePart.GetRefSkeleton();
        if (!ensureMsgf(Captured.Num() == Skeleton.GetNum() && Pose->BoneSpaceTransforms.Num() == Captured.Num(),
            TEXT("[BBBZombieGib]最终姿态和部件骨骼不匹配 Bone=%s"), *Bone.ToString()))
        {
            ReleaseSlot(Slot);
            return false;
        }
        for (int32 Index = 0; Index < Captured.Num(); ++Index)
        {
            const int32 Parent = Skeleton.GetParentIndex(Index);
            Pose->BoneSpaceTransforms[Index] = Parent == INDEX_NONE ? Captured[Index]
                : Captured[Index].GetRelativeTransform(Captured[Parent]);
        }
        Pose->MarkRefreshTransformDirty();
        Pose->RefreshBoneTransforms();
    }
    for (int32 Index = 0; Index < 8; ++Index)
    {
        const float Value = Source.GetCustomPrimitiveData().Data.IsValidIndex(Index) ? Source.GetCustomPrimitiveData().Data[Index] : 0.0f;
        Piece->SetCustomPrimitiveDataFloat(Index, Value);
        Pose->SetCustomPrimitiveDataFloat(Index, Value);
    }
    Piece->SetCustomPrimitiveDataFloat(8, 0.0f);
    Pose->SetCustomPrimitiveDataFloat(8, 0.0f);
    Piece->SetCustomPrimitiveDataFloat(9, 1.0f);
    Pose->SetCustomPrimitiveDataFloat(9, 1.0f);
    Piece->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
    Piece->SetUseCCD(true);
    Piece->SetSimulatePhysics(true);
    if (!Piece->IsSimulatingPhysics())
    {
        UE_LOG(LogTemp, Warning, TEXT("[BBBZombieGib]部件物理未就绪 不保留静止假残留 Mesh=%s"), *Part.GetPathName());
        ReleaseSlot(Slot);
        return false;
    }
    Piece->SetEnableGravity(true);
    float Speed = 260.0f;
    float Lift = 100.0f;
    float SpinSpeed = 180.0f;
    float Mass = 11.0f;
    if (Bone == TEXT("head"))
    {
        Speed = 420.0f;
        Lift = 160.0f;
        SpinSpeed = 340.0f;
        Mass = 4.5f;
    }
    if (Bone == TEXT("upperarm_l") || Bone == TEXT("upperarm_r"))
    {
        Speed = 340.0f;
        Lift = 130.0f;
        SpinSpeed = 260.0f;
        Mass = 3.5f;
    }
    FRandomStream Random(Seed ^ GetTypeHash(Bone));
    const FVector Forward = Direction.GetSafeNormal(UE_SMALL_NUMBER, Source.GetForwardVector());
    const FVector Side = FVector::CrossProduct(Forward, FVector::UpVector).GetSafeNormal(UE_SMALL_NUMBER,
        BoneWorld.GetUnitAxis(EAxis::Y));
    Piece->SetMassOverrideInKg(NAME_None, Mass);
    Piece->SetPhysicsLinearVelocity(Velocity.GetClampedToMaxSize(650.0f)
        + Forward * Speed * Random.FRandRange(0.9f, 1.1f) + FVector::UpVector * Lift
        + Side * Random.FRandRange(-45.0f, 45.0f));
    const FVector Spin = (Side + Forward * Random.FRandRange(-0.35f, 0.35f)).GetSafeNormal() * SpinSpeed;
    Piece->SetPhysicsAngularVelocityInDegrees(Spin);
    Piece->WakeAllRigidBodies();
    BornAt[Slot] = World->GetTimeSeconds();
    StableSince[Slot] = -1.0f;
    SoundConfigs[Slot] = Sounds;
    LandingPlayed[Slot] = false;
    return true;
}

void UBBBMonsterGibPresentationSubsystem::OnPieceHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComponent, const FVector NormalImpulse, const FHitResult& Hit)
{
    const int32 Index = Pieces.IndexOfByPredicate([HitComponent](const auto& Piece)
    {
        return Piece.Get() == HitComponent;
    });
    if (Index == INDEX_NONE || BornAt[Index] < 0.0f || LandingPlayed[Index] || !Hit.bBlockingHit
        || Hit.ImpactNormal.Z < 0.35f || !OtherComponent)
    {
        return;
    }
    LandingPlayed[Index] = true;
    const auto* Audio = SoundConfigs[Index].Get();
    if (Audio && !Audio->Landings.IsEmpty())
    {
        UGameplayStatics::PlaySoundAtLocation(this, Audio->Landings[Index % Audio->Landings.Num()], Hit.ImpactPoint,
            0.35f, 0.9f, 0.0f, Audio->Attenuation, Audio->ContactConcurrency);
    }
}

void UBBBMonsterGibPresentationSubsystem::AdvancePresentation(const float Now)
{
    if (AdvancedFrame == GFrameCounter)
    {
        return;
    }
    AdvancedFrame = GFrameCounter;
    int32 Settled = GetSettledCount();
    for (int32 Index = 0; Index < Pieces.Num(); ++Index)
    {
        if (BornAt[Index] < 0.0f)
        {
            continue;
        }
        auto* Piece = Pieces[Index].Get();
        const float Age = Now - BornAt[Index];
        if (Poses[Index]->IsVisible() && ViewDistanceSquared(*GetWorld(), Piece->GetComponentLocation()) > FMath::Square(1400.0f))
        {
            Poses[Index]->SetVisibility(false);
            Piece->SetVisibility(true);
        }
        const bool bVisible = Piece->WasRecentlyRendered(0.2f) || Poses[Index]->WasRecentlyRendered(0.2f);
        const float Fade = FMath::Clamp((Age - Lifetime) / 5.0f, 0.0f, 1.0f);
        Piece->SetCustomPrimitiveDataFloat(8, Fade);
        Poses[Index]->SetCustomPrimitiveDataFloat(8, Fade);
        if (Age >= Lifetime && (!bVisible || Age >= Lifetime + 5.0f))
        {
            Settled -= Piece->IsSimulatingPhysics() ? 0 : 1;
            ReleaseSlot(Index);
            continue;
        }
        if (!Piece->IsSimulatingPhysics())
        {
            continue;
        }
        const bool bStable = Piece->GetPhysicsLinearVelocity().SizeSquared() < 225.0f
            && Piece->GetPhysicsAngularVelocityInDegrees().SizeSquared() < 225.0f;
        if (!bStable)
        {
            StableSince[Index] = -1.0f;
        }
        if (bStable && StableSince[Index] < 0.0f)
        {
            StableSince[Index] = Now;
        }
        if (Age > 8.0f && !bVisible)
        {
            ReleaseSlot(Index);
            continue;
        }
        if (StableSince[Index] >= 0.0f && Now - StableSince[Index] > 0.5f && Age > 1.0f)
        {
            FHitResult Support;
            const FVector Origin = Piece->Bounds.Origin;
            const float Depth = Piece->Bounds.BoxExtent.Z + 3.0f;
            FCollisionQueryParams Query(SCENE_QUERY_STAT(BBBZombieGibSupport));
            Query.AddIgnoredComponent(Piece);
            FCollisionObjectQueryParams Objects;
            Objects.AddObjectTypesToQuery(ECC_WorldStatic);
            Objects.AddObjectTypesToQuery(ECC_WorldDynamic);
            const bool bSupported = GetWorld()->LineTraceSingleByObjectType(Support, Origin,
                Origin - FVector::UpVector * Depth, Objects, Query) && Support.ImpactNormal.Z >= 0.35f;
            if (!bSupported)
            {
                StableSince[Index] = -1.0f;
                Piece->WakeAllRigidBodies();
                continue;
            }
            Piece->PutAllRigidBodiesToSleep();
            Piece->SetSimulatePhysics(false);
            Piece->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            ++Settled;
            if (Settled > MaximumSettled)
            {
                const int32 Oldest = FindSettledRecycleSlot();
                if (Oldest != INDEX_NONE)
                {
                    ReleaseSlot(Oldest);
                    --Settled;
                }
            }
        }
    }
}

void UBBBMonsterGibPresentationSubsystem::Deinitialize()
{
    AActor* PoolActor = Pieces.IsEmpty() ? nullptr : Pieces[0]->GetOwner();
    for (UPoseableMeshComponent* Pose : Poses)
    {
        Pose->DestroyComponent();
    }
    for (UStaticMeshComponent* Piece : Pieces)
    {
        Piece->DestroyComponent();
    }
    Poses.Reset();
    Pieces.Reset();
    BornAt.Reset();
    StableSince.Reset();
    SoundConfigs.Reset();
    LandingPlayed.Reset();
    ContactMaterial = nullptr;
    if (IsValid(PoolActor))
    {
        PoolActor->Destroy();
    }
    Super::Deinitialize();
}
