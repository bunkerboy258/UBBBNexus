#include "BBBMonsterGibPresentationSubsystem.h"
#include "BBBMonsterBloodImpact.h"

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

bool UBBBMonsterGibPresentationSubsystem::BuildDetachedPose(const FReferenceSkeleton& SourceSkeleton,
    TConstArrayView<FTransform> ComponentPose, const FReferenceSkeleton& PartSkeleton,
    TArray<FTransform>& LocalPose)
{
    LocalPose.Reset();
    if (ComponentPose.Num() != SourceSkeleton.GetNum())
    {
        return false;
    }
    LocalPose.Reserve(PartSkeleton.GetNum());
    for (int32 Index = 0; Index < PartSkeleton.GetNum(); ++Index)
    {
        const int32 SourceIndex = SourceSkeleton.FindBoneIndex(PartSkeleton.GetBoneName(Index));
        const int32 Parent = PartSkeleton.GetParentIndex(Index);
        const int32 SourceParent = Parent == INDEX_NONE ? INDEX_NONE
            : SourceSkeleton.FindBoneIndex(PartSkeleton.GetBoneName(Parent));
        if (!ComponentPose.IsValidIndex(SourceIndex)
            || (Parent != INDEX_NONE && !ComponentPose.IsValidIndex(SourceParent)))
        {
            LocalPose.Reset();
            return false;
        }
        const FTransform Local = Parent == INDEX_NONE ? ComponentPose[SourceIndex]
            : ComponentPose[SourceIndex].GetRelativeTransform(ComponentPose[SourceParent]);
        if (Local.ContainsNaN())
        {
            LocalPose.Reset();
            return false;
        }
        LocalPose.Add(Local);
    }
    return true;
}

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
    if (Poses[Index])
    {
        Poses[Index]->SetVisibility(false);
    }
    FleshSources[Index].Reset();
    FleshInstances[Index].Invalidate();
    FleshPieces[Index] = false;
    MajorFleshPieces[Index] = false;
    LandedAt[Index] = -1;
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
        const bool bVisible = Pieces[Index]->WasRecentlyRendered(0.2f) || (Poses[Index] && Poses[Index]->WasRecentlyRendered(0.2f));
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

int32 UBBBMonsterGibPresentationSubsystem::GetFleshCount() const
{
    int32 Count = 0;
    for (int32 Index = 0; Index < Pieces.Num(); ++Index)
    {
        Count += BornAt[Index] >= 0 && FleshPieces[Index] ? 1 : 0;
    }
    return Count;
}

void UBBBMonsterGibPresentationSubsystem::GetFleshPieces(TArray<UStaticMeshComponent*>& Out) const
{
    Out.Reset();
    for (int32 Index = 0; Index < Pieces.Num(); ++Index)
    {
        if (BornAt[Index] >= 0 && FleshPieces[Index])
        {
            Out.Add(Pieces[Index]);
        }
    }
}

void UBBBMonsterGibPresentationSubsystem::ClearFleshForSource(const AActor& Source)
{
    for (int32 Index = 0; Index < Pieces.Num(); ++Index)
    {
        if (BornAt[Index] >= 0 && FleshPieces[Index] && FleshSources[Index].Get() == &Source)
        {
            ReleaseSlot(Index);
        }
    }
}

int32 UBBBMonsterGibPresentationSubsystem::EmitFlesh(const FBBBMonsterBloodImpact& Impact,
    const UBBBMonsterPresentationDefinition& Settings)
{
    UWorld* World = GetWorld();
    const auto& Flesh = Settings.Gibs.Flesh;
    if (!World || World->GetNetMode() == NM_DedicatedServer || Impact.bDripOnly || !Flesh.Mesh
        || !Flesh.Material || Flesh.Mesh->IsCompiling() || Impact.Position.ContainsNaN()
        || Impact.Normal.ContainsNaN() || Impact.Direction.ContainsNaN()
        || ViewDistanceSquared(*World, Impact.Position) > FMath::Square(1500.0f))
    {
        return 0;
    }
    const bool bMajor = Impact.bSevering || Impact.bWoundOpening;
    const float Now = World->GetTimeSeconds();
    TArray<int32, TInlineAllocator<5>> Recent;
    const float Interval = FMath::Max(Flesh.Interval, 0.1f);
    for (int32 Index = 0; Index < Pieces.Num(); ++Index)
    {
        const bool bSameSource = Impact.InstanceId.IsValid() ? FleshInstances[Index] == Impact.InstanceId
            : Impact.Source.IsValid() && FleshSources[Index] == Impact.Source;
        if (BornAt[Index] < 0 || !FleshPieces[Index] || !bSameSource)
        {
            continue;
        }
        const float Age = Now - BornAt[Index];
        if (MajorFleshPieces[Index] && Age < Interval * 3)
        {
            return 0;
        }
        if (Age < Interval)
        {
            if (!bMajor)
            {
                return 0;
            }
            Recent.Add(Index);
        }
    }
    FRandomStream Random(Impact.Seed ^ 0x31f5a9);
    const FIntPoint Range = bMajor ? Flesh.MajorCount : Flesh.HitCount;
    const int32 Minimum = FMath::Clamp(FMath::Min(Range.X, Range.Y), 0, 5);
    const int32 Maximum = FMath::Clamp(FMath::Max(Range.X, Range.Y), Minimum, 5);
    int32 Requested = bMajor ? Random.RandRange(Minimum, Maximum)
        : (Random.FRand() < 0.75f ? Minimum : Maximum);
    // 同次毁伤只补足目标数量 已经飞出的普通肉块计入本次总量
    for (const int32 Index : Recent)
    {
        MajorFleshPieces[Index] = true;
    }
    Requested = FMath::Max(0, Requested - Recent.Num());
    const FVector Outward = Impact.Normal.GetSafeNormal(UE_SMALL_NUMBER, -Impact.Direction.GetSafeNormal());
    const FVector Side = FVector::CrossProduct(Outward, FVector::UpVector).GetSafeNormal(UE_SMALL_NUMBER, FVector::RightVector);
    if (!ContactMaterial)
    {
        ContactMaterial = NewObject<UPhysicalMaterial>(this);
        ContactMaterial->Friction = 0.65f;
        ContactMaterial->StaticFriction = 0.8f;
        ContactMaterial->Restitution = 0.12f;
    }
    int32 Emitted = 0;
    for (int32 Index = 0; Index < Requested; ++Index)
    {
        const int32 Slot = AcquireSlot(true);
        if (Slot == INDEX_NONE)
        {
            break;
        }
        auto* Piece = Pieces[Slot].Get();
        Piece->EmptyOverrideMaterials();
        Piece->SetStaticMesh(Flesh.Mesh);
        Piece->SetMaterial(0, Flesh.Material);
        Piece->SetPhysMaterialOverride(ContactMaterial);
        const float Scale = Random.FRandRange(FMath::Clamp(static_cast<float>(Flesh.Scale.X), 0.1f, 0.6f),
            FMath::Clamp(static_cast<float>(Flesh.Scale.Y), 0.1f, 0.6f));
        const FQuat Rotation = FRotator(Random.FRandRange(-180, 180), Random.FRandRange(-180, 180), Random.FRandRange(-180, 180)).Quaternion();
        Piece->SetWorldTransform(FTransform(Rotation, Impact.Position + Outward * 8 + Side * Random.FRandRange(-3, 3), FVector(Scale)),
            false, nullptr, ETeleportType::TeleportPhysics);
        Piece->SetCustomPrimitiveDataFloat(8, 0);
        Piece->SetVisibility(true);
        Piece->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
        Piece->SetUseCCD(true);
        Piece->SetSimulatePhysics(true);
        if (!Piece->IsSimulatingPhysics())
        {
            ReleaseSlot(Slot);
            continue;
        }
        Piece->SetEnableGravity(true);
        Piece->SetMassOverrideInKg(NAME_None, 0.08f);
        Piece->SetLinearDamping(0.35f);
        Piece->SetAngularDamping(2.0f);
        const float Speed = Random.FRandRange(FMath::Clamp(static_cast<float>(Flesh.Speed.X), 50.0f, 500.0f),
            FMath::Clamp(static_cast<float>(Flesh.Speed.Y), 50.0f, 500.0f));
        FVector Velocity = Outward * Speed + Side * Random.FRandRange(-70, 70) + FVector::UpVector * Random.FRandRange(15, 45);
        Velocity.Z = FMath::Clamp(Velocity.Z, -120.0, 90.0);
        Piece->SetPhysicsLinearVelocity(Velocity);
        Piece->SetPhysicsAngularVelocityInDegrees(Random.VRand() * Random.FRandRange(180, 360));
        Piece->WakeAllRigidBodies();
        BornAt[Slot] = Now;
        StableSince[Slot] = -1;
        SoundConfigs[Slot] = Settings.Sounds;
        PresentationConfigs[Slot] = &Settings;
        LandingPlayed[Slot] = false;
        MajorFleshPieces[Slot] = bMajor;
        FleshInstances[Slot] = Impact.InstanceId;
        FleshSources[Slot] = Impact.Source;
        ++Emitted;
    }
    return Emitted;
}

int32 UBBBMonsterGibPresentationSubsystem::AcquireSlot(const bool bFlesh)
{
    UWorld* World = GetWorld();
    if (!World || GetSimulatingCount() >= MaximumSimulating
        || (bFlesh && (GetSimulatingCount() >= MaximumSimulating - 8 || GetFleshCount() >= 64)))
    {
        return INDEX_NONE;
    }
    int32 Slot = BornAt.IndexOfByPredicate([](const float Value) { return Value < 0.0f; });
    if (Slot == INDEX_NONE && Pieces.Num() >= MaximumSimulating + MaximumSettled)
    {
        Slot = FindSettledRecycleSlot();
        if (Slot == INDEX_NONE)
        {
            return INDEX_NONE;
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
                return INDEX_NONE;
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
        Slot = Pieces.Add(Piece);
        Poses.Add(nullptr);
        FleshPieces.Add(false);
        MajorFleshPieces.Add(false);
        LandedAt.Add(-1);
        FleshInstances.Add(FGuid());
        FleshSources.Add(nullptr);
        BornAt.Add(-1.0f);
        StableSince.Add(-1.0f);
        SoundConfigs.Add(nullptr);
        PresentationConfigs.Add(nullptr);
        LandingPlayed.Add(false);
    }
    FleshPieces[Slot] = bFlesh;
    LandedAt[Slot] = -1;
    return Slot;
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
    const int32 Slot = AcquireSlot(false);
    if (Slot == INDEX_NONE)
    {
        return false;
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
    if (!Pose)
    {
        AActor* PoolActor = Piece->GetOwner();
        Pose = NewObject<UPoseableMeshComponent>(PoolActor);
        PoolActor->AddInstanceComponent(Pose);
        Pose->PrimaryComponentTick.bCanEverTick = false;
        Pose->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Pose->SetCanEverAffectNavigation(false);
        Pose->SetupAttachment(Piece);
        Pose->RegisterComponentWithWorld(World);
        Poses[Slot] = Pose;
    }
    Piece->EmptyOverrideMaterials();
    Piece->SetLinearDamping(0.25f);
    Piece->SetAngularDamping(0.9f);
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
        if (!ensureMsgf(Source.GetSkeletalMeshAsset()
            && BuildDetachedPose(Source.GetSkeletalMeshAsset()->GetRefSkeleton(), Captured, Skeleton, Pose->BoneSpaceTransforms),
            TEXT("[BBBZombieGib]最终姿态和部件骨骼不匹配 Bone=%s"), *Bone.ToString()))
        {
            ReleaseSlot(Slot);
            return false;
        }
        Pose->MarkRefreshTransformDirty();
        Pose->RefreshBoneTransforms();
    }
    for (int32 Index = 0; Index < 18; ++Index)
    {
        const float Value = Source.GetCustomPrimitiveData().Data.IsValidIndex(Index) ? Source.GetCustomPrimitiveData().Data[Index] : 0.0f;
        if (Index != 8 && Index != 9)
        {
            Piece->SetCustomPrimitiveDataFloat(Index, Value);
            Pose->SetCustomPrimitiveDataFloat(Index, Value);
        }
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
    LandedAt[Index] = GetWorld()->GetTimeSeconds();
    if (FleshPieces[Index])
    {
        Pieces[Index]->SetLinearDamping(4.0f);
        Pieces[Index]->SetAngularDamping(6.0f);
    }
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
        if (Poses[Index] && Poses[Index]->IsVisible() && ViewDistanceSquared(*GetWorld(), Piece->GetComponentLocation()) > FMath::Square(1400.0f))
        {
            Poses[Index]->SetVisibility(false);
            Piece->SetVisibility(true);
        }
        const bool bVisible = Piece->WasRecentlyRendered(0.2f) || (Poses[Index] && Poses[Index]->WasRecentlyRendered(0.2f));
        const bool bFlesh = FleshPieces[Index];
        const float RetainedAge = bFlesh ? (LandedAt[Index] >= 0 ? Now - LandedAt[Index] : 0) : Age;
        const float Lifetime = bFlesh ? FMath::Clamp(Settings.Flesh.Lifetime, 1.0f, 60.0f) : Settings.Lifetime;
        const float FadeDuration = bFlesh ? FMath::Max(Settings.Flesh.FadeDuration, 0.1f) : Settings.FadeDuration;
        const float Fade = FMath::Clamp((RetainedAge - Lifetime) / FadeDuration, 0.0f, 1.0f);
        if (!FMath::IsNearlyEqual(Piece->GetCustomPrimitiveData().Data.IsValidIndex(8)
            ? Piece->GetCustomPrimitiveData().Data[8] : 0.0f, Fade))
        {
            Piece->SetCustomPrimitiveDataFloat(8, Fade);
            if (Poses[Index])
            {
                Poses[Index]->SetCustomPrimitiveDataFloat(8, Fade);
            }
        }
        if ((bFlesh ? RetainedAge >= Lifetime + FadeDuration
                : RetainedAge >= Lifetime && (!bVisible || RetainedAge >= Lifetime + FadeDuration))
            || (bFlesh && LandedAt[Index] < 0 && Age > 8.0f))
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
        if (Pose)
        {
            Pose->DestroyComponent();
        }
    }
    for (UStaticMeshComponent* Piece : Pieces)
    {
        Piece->DestroyComponent();
    }
    Poses.Reset();
    FleshPieces.Reset();
    MajorFleshPieces.Reset();
    LandedAt.Reset();
    FleshInstances.Reset();
    FleshSources.Reset();
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
