#include "BBBMonsterGibPresentationSubsystem.h"

#include "Components/PoseableMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshResources.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSoundPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterPresentationDefinition.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

namespace
{
    constexpr int32 MaximumSimulating = 32;
    constexpr int32 MaximumSettled = 128;

    float ViewDistanceSquared(const UWorld& World, const FVector& Location)
    {
        if (World.WorldType == EWorldType::EditorPreview)
        {
            return 0;
        }
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
    PresentationConfigs[Index] = nullptr;
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
    const uint32 Seed, const UBBBMonsterPresentationDefinition& Settings)
{
    UWorld* World = GetWorld();
    if (!World || Direction.ContainsNaN() || Velocity.ContainsNaN() || Part.IsCompiling()
        || World->GetNetMode() == NM_DedicatedServer || (!Source.WasRecentlyRendered(0.2f) && World->WorldType != EWorldType::EditorPreview)
        || Source.GetPredictedLODLevel() > 2 || Source.bHiddenInGame || (Source.GetOwner() && Source.GetOwner()->IsHidden()))
    {
        return false;
    }
    const FTransform BoneWorld = Source.GetSocketTransform(Bone);
    if (BoneWorld.ContainsNaN() || Source.GetComponentTransform().ContainsNaN()
        || !Part.GetRenderData() || Part.GetRenderData()->Bounds.ContainsNaN() || PosePart.GetBounds().ContainsNaN())
    {
        UE_LOG(LogTemp, Error, TEXT("[BBBZombieGib]拒绝无效几何或变换 Part=%s Pose=%s Bone=%s Transform=%s"),
            *Part.GetPathName(), *PosePart.GetPathName(), *Bone.ToString(), *BoneWorld.ToString());
        return false;
    }
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
        PresentationConfigs.Add(nullptr);
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
            if (Pose->BoneSpaceTransforms[Index].ContainsNaN())
            {
                UE_LOG(LogTemp, Error, TEXT("[BBBZombieGib]拒绝非有限姿态 Part=%s BoneIndex=%d"), *PosePart.GetPathName(), Index);
                ReleaseSlot(Slot);
                return false;
            }
        }
        Pose->MarkRefreshTransformDirty();
        Pose->RefreshBoneTransforms();
    }
    for (int32 Index = 0; Index < 36; ++Index)
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
    const auto& Gibs = Settings.Gibs;
    const auto& PartSettings = Gibs.ReadPart(Bone);
    FRandomStream Random(Seed ^ GetTypeHash(Bone));
    const FVector Forward = Direction.GetSafeNormal(UE_SMALL_NUMBER, Source.GetForwardVector());
    const FVector Side = FVector::CrossProduct(Forward, FVector::UpVector).GetSafeNormal(UE_SMALL_NUMBER,
        BoneWorld.GetUnitAxis(EAxis::Y));
    Piece->SetMassOverrideInKg(NAME_None, PartSettings.Mass);
    Piece->SetPhysicsLinearVelocity((Velocity * Gibs.InheritedVelocityScale).GetClampedToMaxSize(Gibs.MaximumInheritedSpeed)
        + Forward * PartSettings.Speed * Random.FRandRange(Gibs.SpeedScale.X, Gibs.SpeedScale.Y) + FVector::UpVector * PartSettings.Lift
        + Side * Random.FRandRange(-Gibs.SideSpeed, Gibs.SideSpeed));
    const FVector Spin = (Side + Forward * Random.FRandRange(-Gibs.SpinDirectionSpread, Gibs.SpinDirectionSpread)).GetSafeNormal() * PartSettings.Spin;
    Piece->SetPhysicsAngularVelocityInDegrees(Spin);
    Piece->WakeAllRigidBodies();
    BornAt[Slot] = World->GetTimeSeconds();
    StableSince[Slot] = -1.0f;
    SoundConfigs[Slot] = Settings.Sounds;
    PresentationConfigs[Slot] = &Settings;
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
    if (!FMath::IsFinite(Now) || Now <= AdvancedTime)
    {
        return;
    }
    AdvancedTime = Now;
    int32 Settled = GetSettledCount();
    for (int32 Index = 0; Index < Pieces.Num(); ++Index)
    {
        if (BornAt[Index] < 0.0f)
        {
            continue;
        }
        auto* Piece = Pieces[Index].Get();
        const float Age = Now - BornAt[Index];
        const auto& Settings = PresentationConfigs[Index]->Gibs;
        if (Poses[Index]->IsVisible() && ViewDistanceSquared(*GetWorld(), Piece->GetComponentLocation()) > FMath::Square(1400.0f))
        {
            Poses[Index]->SetVisibility(false);
            Piece->SetVisibility(true);
        }
        const bool bVisible = Piece->WasRecentlyRendered(0.2f) || Poses[Index]->WasRecentlyRendered(0.2f);
        const float Fade = FMath::Clamp((Age - Settings.Lifetime) / Settings.FadeDuration, 0.0f, 1.0f);
        Piece->SetCustomPrimitiveDataFloat(8, Fade);
        Poses[Index]->SetCustomPrimitiveDataFloat(8, Fade);
        if (Age >= Settings.Lifetime && (!bVisible || Age >= Settings.Lifetime + Settings.FadeDuration))
        {
            Settled -= Piece->IsSimulatingPhysics() ? 0 : 1;
            ReleaseSlot(Index);
            continue;
        }
        if (!Piece->IsSimulatingPhysics())
        {
            continue;
        }
        const bool bStable = Piece->GetPhysicsLinearVelocity().SizeSquared() < FMath::Square(Settings.RestLinearSpeed)
            && Piece->GetPhysicsAngularVelocityInDegrees().SizeSquared() < FMath::Square(Settings.RestAngularSpeed);
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
        if (StableSince[Index] >= 0.0f && Now - StableSince[Index] > Settings.RestDuration && Age > 1.0f)
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
    PresentationConfigs.Reset();
    LandingPlayed.Reset();
    ContactMaterial = nullptr;
    if (IsValid(PoolActor))
    {
        PoolActor->Destroy();
    }
    Super::Deinitialize();
}
