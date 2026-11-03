#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Collision/BBBMonsterCollisionProcessor.h"

#include "MassExecutionContext.h"
#include "MassCommonFragments.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterAvoidanceFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterAvoidanceProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitRegion.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "MassActorSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "MassEntitySubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterGoreDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterGoreFragment.h"
#include "Engine/SkeletalMesh.h"

namespace
{
    /** 只在查询候选时读取当前姿势 无表现载体时使用完整逻辑部位形状 */
    void VisitBodies(UWorld& World, const FMassEntityHandle Entity,
        const TFunctionRef<void(const FBBBMassCollisionBody&)> Visitor)
    {
        const auto* Entities = World.GetSubsystem<UMassEntitySubsystem>();
        if (!IsInGameThread() || !Entities)
        {
            return;
        }
        const auto& Manager = Entities->GetEntityManager();
        if (!Manager.IsEntityValid(Entity))
        {
            return;
        }
        const auto* Health = Manager.GetFragmentDataPtr<FBBBMonsterHealthFragment>(Entity);
        const auto* TransformData = Manager.GetFragmentDataPtr<FTransformFragment>(Entity);
        const auto* Avoidance = Manager.GetFragmentDataPtr<FBBBMonsterAvoidanceFragment>(Entity);
        const auto* Mobility = Manager.GetFragmentDataPtr<FBBBMonsterMobilityFragment>(Entity);
        const auto* Network = Manager.GetFragmentDataPtr<FBBBMonsterNetworkFragment>(Entity);
        if (!Health || !TransformData || !Avoidance || !Mobility || !Network)
        {
            return;
        }
        const auto* ActorData = Manager.GetFragmentDataPtr<FMassActorFragment>(Entity);
        const auto* Actor = ActorData ? Cast<ABBBMonsterPresentationActor>(ActorData->Get()) : nullptr;
        const auto* Mesh = Actor ? Actor->GetMonsterMesh() : nullptr;
        if (Health->CurrentHealth <= 0.0f && !Mesh)
        {
            return;
        }
        const auto* Gore = Manager.GetFragmentDataPtr<FBBBMonsterGoreFragment>(Entity);
        const auto* Settings = Network->Definition.IsValid() ? Network->Definition->Presentation.Get() : nullptr;
        const auto* Wounds = Settings ? Settings->Gore.Get() : nullptr;
        const auto& Transform = TransformData->GetTransform();
        const float Scale = FMath::Clamp(Avoidance->CollisionRadius / 45.0f, 0.5f, 2.0f);
        const auto* Definition = Network->Definition.Get();
        const float CrawlProgress = Mobility->bCrawling
            ? FMath::Clamp((World.GetTimeSeconds() - Mobility->CrawlStartedAt) / (Definition ? Definition->CrawlTransitionDuration : 1.0f), 0.0f, 1.0f)
            : 0.0f;
        FPlane CutPlanes[6];
        uint8 CutMask = 0;
        if (Gore && Wounds && Wounds->Body && Wounds->Dismemberment && Mesh)
        {
            const auto& Skeleton = Wounds->Body->GetRefSkeleton();
            for (int32 Region = 1; Region < 6; ++Region)
            {
                const auto* Part = Wounds->Dismemberment->Find(Gore->DismembermentPresets[Region - 1]);
                if (!Part)
                {
                    continue;
                }
                const int32 Bone = Skeleton.FindBoneIndex(Part->Bone);
                if (Bone == INDEX_NONE || Mesh->GetBoneIndex(Part->Bone) == INDEX_NONE)
                {
                    continue;
                }
                FTransform Reference = Skeleton.GetRefBonePose()[Bone];
                for (int32 Parent = Skeleton.GetParentIndex(Bone); Parent != INDEX_NONE; Parent = Skeleton.GetParentIndex(Parent))
                {
                    Reference *= Skeleton.GetRefBonePose()[Parent];
                }
                const FTransform Current = Mesh->GetSocketTransform(Part->Bone);
                const FVector Point = Current.TransformPosition(Reference.InverseTransformPosition(Part->Contact));
                const FVector Axis = Current.TransformVectorNoScale(Reference.InverseTransformVectorNoScale(
                    Part->Rotation.RotateVector(FVector::ForwardVector))).GetSafeNormal();
                CutPlanes[Region] = FPlane(Point, Axis);
                CutMask |= 1u << Region;
            }
        }
        const auto BonePoint = [Mesh, &Transform, Scale, CrawlProgress](const FName Bone, const FVector& Fallback)
        {
            if (Mesh && Mesh->GetBoneIndex(Bone) != INDEX_NONE)
            {
                return Mesh->GetBoneLocation(Bone, EBoneSpaces::WorldSpace);
            }
            const FVector Prone(Fallback.Z, Fallback.Y, -20.0f + Fallback.X);
            return Transform.TransformPosition(FMath::Lerp(Fallback, Prone, CrawlProgress) * Scale);
        };
        const auto Sphere = [Entity, Scale, &CutPlanes, CutMask, Visitor](const FVector& Center, const float Radius, const EBBBMonsterHitRegion Region)
        {
            if ((CutMask & (1u << static_cast<uint8>(Region))) != 0
                && CutPlanes[static_cast<int32>(Region)].PlaneDot(Center) >= 0)
            {
                return;
            }
            FBBBMassCollisionBody Body;
            Body.Entity = Entity;
            Body.Center = Center;
            Body.Radius = Radius * Scale;
            Body.Surface = SurfaceType2;
            Body.Part = static_cast<uint8>(Region);
            Visitor(Body);
        };
        if (!Definition)
        {
            return;
        }
        for (const auto& Shape : Definition->HitShapes)
        {
            if (!Shape.IsValid())
            {
                continue;
            }
            if (Shape.EndBone.IsNone())
            {
                const FVector Center = Mesh && Mesh->GetBoneIndex(Shape.Bone) != INDEX_NONE
                    ? Mesh->GetSocketTransform(Shape.Bone).TransformPosition(Shape.Offset)
                    : BonePoint(Shape.Bone, Shape.FallbackStart + Shape.Offset);
                Sphere(Center, Shape.Radius, Shape.Region);
                continue;
            }
            const FVector A = BonePoint(Shape.Bone, Shape.FallbackStart);
            const FVector B = BonePoint(Shape.EndBone, Shape.FallbackEnd);
            const int32 Steps = FMath::Clamp(FMath::CeilToInt(FVector::Distance(A, B) / Shape.Radius), 1, 8);
            for (int32 Step = 0; Step <= Steps; ++Step)
            {
                Sphere(FMath::Lerp(A, B, float(Step) / Steps), Shape.Radius, Shape.Region);
            }
        }
    }
}

bool UBBBMonsterCollisionProcessor::TraceCompound(UWorld& World, const FMassEntityHandle Entity,
    const FVector& Start, const FVector& End, const float Radius, FBBBMassCollisionBody& HitBody, float& HitTime)
{
    HitTime = 1.0f;
    bool bHit = false;
    const FVector Delta = End - Start;
    const double LengthSquared = Delta.SizeSquared();
    if (LengthSquared <= UE_SMALL_NUMBER)
    {
        return false;
    }
    VisitBodies(World, Entity, [&](const FBBBMassCollisionBody& Body)
    {
        const FVector Offset = Start - Body.Center;
        const double CombinedRadius = Radius + Body.Radius;
        const double C = Offset.SizeSquared() - CombinedRadius * CombinedRadius;
        const double B = FVector::DotProduct(Offset, Delta);
        const double Discriminant = B * B - LengthSquared * C;
        if (Discriminant >= 0.0)
        {
            const double Time = C <= 0.0 ? 0.0 : (-B - FMath::Sqrt(Discriminant)) / LengthSquared;
            if (Time >= 0.0 && Time <= HitTime)
            {
                HitTime = Time;
                HitBody = Body;
                bHit = true;
            }
        }
    });
    return bHit;
}

bool UBBBMonsterCollisionProcessor::OverlapCompound(UWorld& World, const FMassEntityHandle Entity,
    const FVector& Center, const float Radius, FBBBMassCollisionBody& HitBody)
{
    bool bHit = false;
    double ClosestDistance = Radius;
    VisitBodies(World, Entity, [&](const FBBBMassCollisionBody& Body)
    {
        const double Distance = FMath::Max(0.0, FVector::Distance(Center, Body.Center) - Body.Radius);
        if (Distance <= ClosestDistance)
        {
            ClosestDistance = Distance;
            HitBody = Body;
            bHit = true;
        }
    });
    return bHit;
}

UBBBMonsterCollisionProcessor::UBBBMonsterCollisionProcessor()
    : EntityQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    QueryBasedPruning = EMassQueryBasedPruning::Never;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Collision;
    ExecutionOrder.ExecuteAfter.Add(BBBMassProcessingGroups::Movement);
}

void UBBBMonsterCollisionProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FBBBMonsterAvoidanceFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
}

void UBBBMonsterCollisionProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    UBBBMassSubsystem* Mass = Context.GetWorld()->GetSubsystem<UBBBMassSubsystem>();
    Mass->BeginCollisionFrame();
    EntityQuery.ForEachEntityChunk(Context, [Mass](FMassExecutionContext& Chunk)
    {
        const auto Transforms = Chunk.GetFragmentView<FTransformFragment>();
        const auto Avoidance = Chunk.GetFragmentView<FBBBMonsterAvoidanceFragment>();
        const auto Health = Chunk.GetFragmentView<FBBBMonsterHealthFragment>();
        const auto Actors = Chunk.GetFragmentView<FMassActorFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            FBBBMassCollisionBody Body;
            Body.Entity = Chunk.GetEntity(Index);
            Body.Surface = SurfaceType2;
            const FTransform& Transform = Transforms[Index].GetTransform();
            const float Scale = FMath::Clamp(Avoidance[Index].CollisionRadius / 45.0f, 0.5f, 2.0f);
            Body.Center = Transform.GetLocation();
            Body.Radius = 200.0f * Scale;
            if (Health[Index].CurrentHealth <= 0.0f)
            {
                const auto* Actor = Actors.IsEmpty() ? nullptr : Cast<ABBBMonsterPresentationActor>(Actors[Index].Get());
                const auto* Mesh = Actor ? Actor->GetMonsterMesh() : nullptr;
                if (!Mesh || !Mesh->IsVisible() || Mesh->bHiddenInGame || Actor->IsHidden())
                {
                    continue;
                }
                Body.Center = Mesh->Bounds.Origin;
                Body.Radius = Mesh->Bounds.SphereRadius + 30.0f;
            }
            Body.bCompound = true;
            Mass->AddCollisionBody(Body);
        }
    });
}
