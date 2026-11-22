#include "BBBMonsterCrowdPresentationSubsystem.h"

#include "Animation/AnimSequence.h"
#include "Animation/AnimSequenceTransformProviderBuilder.h"
#include "Components/InstancedSkinnedMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "ProfilingDebugging/CountersTrace.h"
#include "SkeletalRenderPublic.h"
#include "UObject/ConstructorHelpers.h"

TRACE_DECLARE_INT_COUNTER(BBBMonsterCrowdInstances, TEXT("BBBMonster/CrowdInstances"));
TRACE_DECLARE_INT_COUNTER(BBBMonsterCrowdComponents, TEXT("BBBMonster/CrowdComponents"));

UBBBMonsterCrowdPresentationSubsystem::UBBBMonsterCrowdPresentationSubsystem()
{
    const auto ReadClips = [](const TCHAR* Family, TArray<TObjectPtr<UAnimSequence>>& Clips)
    {
        const TCHAR* Paths[] =
        {
            TEXT("Ramster/Zombie_Idle_01"),
            TEXT("Ramster/Zombie_Idle_02"),
            TEXT("Ramster/Zombie_Idle_03"),
            TEXT("Ramster/Zombie_Walk_03_Cycle"),
            TEXT("Ramster/Zombie_Walk_03_Cycle"),
            TEXT("Ramster/Zombie_Walk_03_Cycle"),
            TEXT("Ramster/Zombie_Walk_Fast01_Forward_InPlace"),
            TEXT("Ramster/Zombie_Walk_02_Forward_InPlace"),
            TEXT("Ramster/Zombie_Walk_Fast01_Forward_InPlace"),
            TEXT("Ramster/Zombie_Run_01_Forward_InPlace"),
            TEXT("Ramster/Zombie_Run_02_Forward_InPlace"),
            TEXT("Ramster/Zombie_Run_01_Forward_InPlace"),
            TEXT("Combat/Zombie_Death_Back")
        };
        for (const TCHAR* Relative : Paths)
        {
            const FString Package = FString::Printf(TEXT("/Game/_Project/Mass/Monster/Zombie/%s/Animations/%s"),
                Family, Relative);
            const FString Path = Package + TEXT(".") + FPaths::GetCleanFilename(Package);
            ConstructorHelpers::FObjectFinder<UAnimSequence> Asset(*Path);
            Clips.Add(Asset.Object);
        }
    };
    ReadClips(TEXT("Male"), MaleClips);
    ReadClips(TEXT("Female"), FemaleClips);
}

UInstancedSkinnedMeshComponent* UBBBMonsterCrowdPresentationSubsystem::GetOrCreateComponent(USkeletalMesh& Mesh)
{
    if (auto* Existing = Components.FindRef(&Mesh).Get())
    {
        return Existing;
    }
    if (UnsupportedBodies.Contains(&Mesh) || !IsGPUSkinSceneExtensionEnabled() || Mesh.GetLODNum() <= 0)
    {
        return nullptr;
    }

    // 与正式动画资源使用同一骨架的身体才启用远处实例
    const auto Matches = [&Mesh](const TArray<TObjectPtr<UAnimSequence>>& Clips)
    {
        return !Clips.IsEmpty() && !Clips.ContainsByPredicate([&Mesh](const UAnimSequence* Clip)
        {
            return !Clip || Clip->GetSkeleton() != Mesh.GetSkeleton() || Clip->GetPlayLength() <= 0;
        });
    };
    const auto* Clips = Matches(MaleClips) ? &MaleClips : Matches(FemaleClips) ? &FemaleClips : nullptr;
    if (!Clips)
    {
        UnsupportedBodies.Add(&Mesh);
        return nullptr;
    }

    auto* Component = NewObject<UInstancedSkinnedMeshComponent>(this);
    Component->SetMobility(EComponentMobility::Movable);
    Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Component->SetGenerateOverlapEvents(false);
    Component->SetCastShadow(false);
    Component->SetVisibleInRayTracing(false);
    Component->SetSkinnedAssetAndUpdate(&Mesh);
    Component->OverrideMinLOD(FMath::Min(2, Mesh.GetLODNum() - 1));
    Component->PrimaryComponentTick.bCanEverTick = false;

    auto* Builder = UAnimSequenceTransformProviderBuilder::MakeBuilder(&Mesh);
    Builder->SetBoundsQuality(EAnimSequenceBoundsQuality::PerSequence);
    for (UAnimSequence* Clip : *Clips)
    {
        FAnimSequenceTransformProviderSequence Sequence;
        Sequence.Sequence = Clip;
        Builder->AddSequence(Sequence);
    }
    TArray<FText> Errors;
    auto* Provider = Builder->Build(Component, Errors);
    if (!Provider)
    {
        for (const auto& Error : Errors)
        {
            UE_LOG(LogTemp, Warning, TEXT("小怪远处实例 %s: %s"), *Mesh.GetPathName(), *Error.ToString());
        }
        UnsupportedBodies.Add(&Mesh);
        return nullptr;
    }

    Component->SetTransformProvider(Provider);
    Component->RegisterComponentWithWorld(GetWorld());
    Components.Add(&Mesh, Component);
    return Component;
}

bool UBBBMonsterCrowdPresentationSubsystem::HasPresentation(FMassEntityHandle Entity) const
{
    return Instances.Contains(Entity);
}

bool UBBBMonsterCrowdPresentationSubsystem::Present(FMassEntityHandle Entity, USkeletalMesh& Mesh, const FTransform& Target,
    int32 Sequence, float Phase, bool bFrozen, float Alpha, float DeltaSeconds, FTransform& Display)
{
    auto* Component = GetOrCreateComponent(Mesh);
    auto* Provider = Component ? Cast<UAnimSequenceTransformProviderDataInstance>(Component->GetTransformProvider()) : nullptr;
    if (!Provider)
    {
        return false;
    }
    const float Length = Provider->GetSequencePlayLength(Sequence);
    if (Length <= 0.0f)
    {
        return false;
    }

    auto& Instance = Instances.FindOrAdd(Entity);
    if (Instance.Component.Get() != Component)
    {
        RemoveInstance(Instance);
        Instance = {};
        Instance.Component = Component;
        Instance.Display = Target;
        Instance.Track = Provider->AllocateTrack();
        if (Instance.Track == INDEX_NONE)
        {
            Instances.Remove(Entity);
            return false;
        }
    }
    Instance.SeenFrame = Frame;
    const double Distance = FVector::DistSquared(Instance.Display.GetLocation(), Target.GetLocation());
    const float Weight = Distance > FMath::Square(500.0) ? 1.0f : Alpha;
    FTransform Next;
    Next.Blend(Instance.Display, Target, Weight);
    const float Position = bFrozen ? Length : FMath::Clamp(Phase, 0.0f, 0.9999f) * Length;
    if (Instance.Sequence != Sequence)
    {
        FAnimSequenceTrackManualData Track;
        Track.SequenceIndex = Sequence;
        Track.Position = Position;
        Track.BlendTime = Instance.Sequence == INDEX_NONE || bFrozen ? 0.0f : 0.12f;
        Track.LoopMode = bFrozen ? EAnimSequenceTrackLoopMode::Clamp : EAnimSequenceTrackLoopMode::Loop;
        if (!Provider->SetManualData(Instance.Track, 0, Track))
        {
            RemoveInstance(Instance);
            Instances.Remove(Entity);
            return false;
        }
        Instance.Sequence = Sequence;
        Instance.PositionSeconds = Position;
    }
    else if (!bFrozen)
    {
        // 相位偏差跨周期取最短方向 播放速率仍受源动画允许区间约束
        const float Expected = Instance.PositionSeconds + DeltaSeconds;
        const float Offset = Position - Expected;
        const float Correction = Offset - FMath::RoundToFloat(Offset / Length) * Length;
        const float Step = FMath::Clamp(DeltaSeconds + Correction, DeltaSeconds * 0.8f, DeltaSeconds * 1.2f);
        Instance.PositionSeconds = FMath::Fmod(Instance.PositionSeconds + Step, Length);
        Provider->SetManualPosition(Instance.Track, 0, Instance.PositionSeconds);
    }

    if (!Instance.Id.IsValid())
    {
        Instance.Id = Component->AddInstance(Next, Instance.Track, true);
    }
    else if (!Next.Equals(Instance.Display, 0.02))
    {
        Component->SetInstanceTransform(Instance.Id, Next, true);
    }
    Instance.Display = Next;
    Display = Next;
    return Instance.Id.IsValid();
}

void UBBBMonsterCrowdPresentationSubsystem::RemoveInstance(const FBBBMonsterCrowdInstance& Instance)
{
    if (auto* Component = Instance.Component.Get())
    {
        if (Instance.Id.IsValid())
        {
            Component->RemoveInstance(Instance.Id);
        }
        if (auto* Provider = Cast<UAnimSequenceTransformProviderDataInstance>(Component->GetTransformProvider()))
        {
            if (Instance.Track != INDEX_NONE)
            {
                Provider->DeallocateTrack(Instance.Track);
            }
        }
    }
}

void UBBBMonsterCrowdPresentationSubsystem::EndFrame()
{
    for (auto It = Instances.CreateIterator(); It; ++It)
    {
        if (It.Value().SeenFrame != Frame)
        {
            RemoveInstance(It.Value());
            It.RemoveCurrent();
        }
    }
    const float Now = GetWorld()->GetTimeSeconds();
    for (auto It = Components.CreateIterator(); It; ++It)
    {
        auto* Component = It.Value().Get();
        if (Component->GetInstanceCount() != 0)
        {
            EmptySince.Remove(Component);
        }
        else if (Now - EmptySince.FindOrAdd(Component, Now) > 6.0f)
        {
            EmptySince.Remove(Component);
            Component->DestroyComponent();
            It.RemoveCurrent();
        }
    }
    ++Frame;
    TRACE_COUNTER_SET(BBBMonsterCrowdInstances, Instances.Num());
    TRACE_COUNTER_SET(BBBMonsterCrowdComponents, Components.Num());
}

void UBBBMonsterCrowdPresentationSubsystem::Deinitialize()
{
    for (const auto& Pair : Components)
    {
        Pair.Value->DestroyComponent();
    }
    Components.Reset();
    Instances.Reset();
    EmptySince.Reset();
    UnsupportedBodies.Reset();
    Super::Deinitialize();
}
