#include "BBBWork/UBBBNexus/Notify/Equipment/Display/BBBEquipmentDisplaySubsystem.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Misc/App.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundWave.h"
#include "AudioMixerBlueprintLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraWorldManager.h"
#include "UObject/UnrealType.h"
#include "PhysicsEngine/BodySetup.h"

bool UBBBEquipmentDisplaySubsystem::DoesSupportWorldType(const EWorldType::Type Type) const
{
    return Type == EWorldType::Game || Type == EWorldType::PIE || Type == EWorldType::EditorPreview;
}

void UBBBEquipmentDisplaySubsystem::Prepare(const UObject &Definition)
{
    if (GetWorld()->GetNetMode() == NM_DedicatedServer || !FApp::CanEverRender())
    {
        return;
    }
    for (TFieldIterator<FObjectPropertyBase> It(Definition.GetClass()); It; ++It)
    {
        auto *Animation = Cast<UAnimSequenceBase>(It->GetObjectPropertyValue_InContainer(&Definition));
        if (Animation && !PreparedAnimations.Contains(Animation))
        {
            PendingAnimations.AddUnique(Animation);
        }
    }
}

void UBBBEquipmentDisplaySubsystem::PrepareMesh(UStaticMesh *Mesh)
{
    const auto *Body = Mesh ? Mesh->GetBodySetup() : nullptr;
    if (Body && Body->AggGeom.GetElementCount() > 0)
    {
        PendingMeshes.AddUnique(Mesh);
    }
}

bool UBBBEquipmentDisplaySubsystem::IsTickable() const
{
    return IsInitialized() && (!PendingAnimations.IsEmpty() || !PendingMeshes.IsEmpty()
        || ExpiresAt.ContainsByPredicate([](double Time) { return Time >= 0; }));
}

TStatId UBBBEquipmentDisplaySubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UBBBEquipmentDisplaySubsystem, STATGROUP_Tickables);
}

void UBBBEquipmentDisplaySubsystem::Release(const int32 Index)
{
    ExpiresAt[Index] = -1;
    if (auto *Actor = Objects[Index].Get(); IsValid(Actor))
    {
        auto *Mesh = Actor->GetStaticMeshComponent();
        Mesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
        Mesh->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector);
        Mesh->PutAllRigidBodiesToSleep();
        Actor->SetActorEnableCollision(false);
        Actor->SetActorHiddenInGame(true);
    }
}

bool UBBBEquipmentDisplaySubsystem::Emit(UStaticMesh *Mesh, const FTransform &Transform, const FVector &Linear,
    const FVector &Angular, const float Lifetime, const bool bStaticOnly)
{
    if (!Mesh || GetWorld()->GetNetMode() == NM_DedicatedServer || !FApp::CanEverRender())
    {
        return false;
    }
    const auto &Views = GetWorld()->ViewLocationsRenderedLastFrame;
    if (!Views.IsEmpty() && !Views.ContainsByPredicate([&](const FVector &View)
        { return FVector::DistSquared(View, Transform.GetLocation()) <= FMath::Square(3000.0); }))
    {
        return false;
    }
    int32 Slot = INDEX_NONE;
    for (int32 Index = 0; Index < Objects.Num(); ++Index)
    {
        if (IsValid(Objects[Index]) && Objects[Index]->GetStaticMeshComponent()->GetStaticMesh() == Mesh
            && (Slot == INDEX_NONE || ExpiresAt[Index] < ExpiresAt[Slot]))
        {
            Slot = Index;
        }
    }
    if (Slot == INDEX_NONE)
    {
        PrepareMesh(Mesh);
        return false;
    }
    Release(Slot);
    auto *Actor = Objects[Slot].Get();
    auto *Component = Actor->GetStaticMeshComponent();
    Component->SetCollisionResponseToAllChannels(bStaticOnly ? ECR_Ignore : ECR_Block);
    Component->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    Component->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    Actor->SetActorTransform(Transform, false, nullptr, ETeleportType::TeleportPhysics);
    Actor->SetActorHiddenInGame(false);
    Actor->SetActorEnableCollision(true);
    Component->SetPhysicsLinearVelocity(Linear);
    Component->SetPhysicsAngularVelocityInRadians(Angular);
    Component->WakeAllRigidBodies();
    ExpiresAt[Slot] = GetWorld()->GetTimeSeconds() + Lifetime;
    return true;
}

void UBBBEquipmentDisplaySubsystem::Tick(float)
{
    const double Now = GetWorld()->GetTimeSeconds();
    for (int32 Index = 0; Index < Objects.Num(); ++Index)
    {
        if (ExpiresAt[Index] >= 0 && ExpiresAt[Index] <= Now)
        {
            Release(Index);
        }
    }
    if (!PendingAnimations.IsEmpty())
    {
        auto *Animation = PendingAnimations[0].Get();
        PendingAnimations.RemoveAt(0);
        PreparedAnimations.Add(Animation);
        if (auto *Montage = Cast<UAnimMontage>(Animation))
        {
            for (const auto &Track : Montage->SlotAnimTracks)
            {
                for (const auto &Segment : Track.AnimTrack.AnimSegments)
                {
                    auto *Child = Segment.GetAnimReference().Get();
                    if (Child && !PreparedAnimations.Contains(Child))
                    {
                        PendingAnimations.AddUnique(Child);
                    }
                }
            }
        }
        for (const auto &Event : Animation->Notifies)
        {
            const UObject *Notify = Event.NotifyStateClass ? static_cast<UObject *>(Event.NotifyStateClass.Get()) : Event.Notify.Get();
            if (!Notify)
            {
                continue;
            }
            for (TFieldIterator<FObjectPropertyBase> It(Notify->GetClass()); It; ++It)
            {
                UObject *Resource = It->GetObjectPropertyValue_InContainer(Notify);
                if (!Resource || PreparedEffects.Contains(Resource))
                {
                    continue;
                }
                PreparedEffects.Add(Resource);
                if (auto *Mesh = Cast<UStaticMesh>(Resource))
                {
                    PrepareMesh(Mesh);
                }
                if (auto *Effect = Cast<UNiagaraSystem>(Resource))
                {
                    if (auto *Manager = FNiagaraWorldManager::Get(GetWorld()))
                    {
                        if (auto *Pool = Manager->GetComponentPool())
                        {
                            Pool->PrimePool(Effect, GetWorld());
                        }
                    }
                }
                if (auto *Sound = Cast<USoundWave>(Resource))
                {
                    UAudioMixerBlueprintLibrary::PrimeSoundForPlayback(Sound, FOnSoundLoadComplete());
                }
                if (auto *Cue = Cast<USoundCue>(Resource))
                {
                    UAudioMixerBlueprintLibrary::PrimeSoundCueForPlayback(Cue);
                }
            }
        }
    }
    if (PendingMeshes.IsEmpty())
    {
        return;
    }
    UStaticMesh *Mesh = PendingMeshes[0];
    int32 Count = 0;
    for (const auto &Object : Objects)
    {
        auto *Actor = Object.Get();
        Count += IsValid(Actor) && Actor->GetStaticMeshComponent()->GetStaticMesh() == Mesh ? 1 : 0;
    }
    if (Count >= 4)
    {
        PendingMeshes.RemoveAt(0);
        return;
    }
    if (Objects.Num() >= 64)
    {
        for (int32 Index = 0; Index < Objects.Num(); ++Index)
        {
            if (ExpiresAt[Index] < 0 && IsValid(Objects[Index])
                && Objects[Index]->GetStaticMeshComponent()->GetStaticMesh() != Mesh)
            {
                auto *Component = Objects[Index]->GetStaticMeshComponent();
                Component->SetStaticMesh(Mesh);
                Component->SetSimulatePhysics(true);
                Release(Index);
                return;
            }
        }
        return;
    }
    FActorSpawnParameters Parameters;
    Parameters.ObjectFlags |= RF_Transient;
    Parameters.bDeferConstruction = true;
    Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    auto *Actor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FTransform::Identity, Parameters);
    if (!Actor)
    {
        return;
    }
    Actor->SetReplicates(false);
    Actor->SetActorTickEnabled(false);
    auto *Component = Actor->GetStaticMeshComponent();
    Component->SetMobility(EComponentMobility::Movable);
    Component->SetStaticMesh(Mesh);
    Component->SetGenerateOverlapEvents(false);
    Component->SetCanEverAffectNavigation(false);
    Component->SetCastShadow(false);
    Component->SetCollisionObjectType(ECC_PhysicsBody);
    Component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Actor->SetActorEnableCollision(false);
    Actor->SetActorHiddenInGame(true);
    Actor->FinishSpawning(FTransform::Identity);
    Component->SetSimulatePhysics(true);
    const int32 Index = Objects.Add(Actor);
    ExpiresAt.Add(-1);
    Release(Index);
}

void UBBBEquipmentDisplaySubsystem::Deinitialize()
{
    for (const auto &Object : Objects)
    {
        auto *Actor = Object.Get();
        if (IsValid(Actor))
        {
            Actor->Destroy();
        }
    }
    Objects.Reset();
    ExpiresAt.Reset();
    PendingAnimations.Reset();
    PendingMeshes.Reset();
    PreparedAnimations.Reset();
    PreparedEffects.Reset();
    Super::Deinitialize();
}
