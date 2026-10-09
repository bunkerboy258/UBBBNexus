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
        if (!Health || Health->CurrentHealth <= 0.0f || !TransformData || !Avoidance || !Mobility || !Network)
        {
            return;
        }
        const auto* ActorData = Manager.GetFragmentDataPtr<FMassActorFragment>(Entity);
        const auto* Actor = ActorData ? Cast<ABBBMonsterPresentationActor>(ActorData->Get()) : nullptr;
        const auto* Mesh = Actor ? Actor->GetMonsterMesh() : nullptr;
        const auto& Transform = TransformData->GetTransform();
        const float Scale = FMath::Clamp(Avoidance->CollisionRadius / 45.0f, 0.5f, 2.0f);
        const auto* Definition = Network->Definition.Get();
        const float CrawlProgress = Mobility->bCrawling
            ? FMath::Clamp((World.GetTimeSeconds() - Mobility->CrawlStartedAt) / (Definition ? Definition->CrawlTransitionDuration : 1.0f), 0.0f, 1.0f)
            : 0.0f;
        const auto BonePoint = [Mesh, &Transform, Scale, CrawlProgress](const FName Bone, const FVector& Fallback)
        {
            if (Mesh && Mesh->GetBoneIndex(Bone) != INDEX_NONE)
            {
                const FVector Local = Mesh->GetRelativeTransform().TransformPosition(Mesh->GetBoneLocation(Bone, EBoneSpaces::ComponentSpace));
                return Transform.TransformPosition(Local);
            }
            const FVector Prone(Fallback.Z, Fallback.Y, -20.0f + Fallback.X);
            return Transform.TransformPosition(FMath::Lerp(Fallback, Prone, CrawlProgress) * Scale);
        };
        const auto Sphere = [Entity, Scale, Health, Visitor](const FVector& Center, const float Radius, const EBBBMonsterHitRegion Region)
        {
            if ((Health->DestroyedParts & (1u << static_cast<uint8>(Region))) == 0)
            {
                FBBBMassCollisionBody Body;
                Body.Entity = Entity;
                Body.Center = Center;
                Body.Radius = Radius * Scale;
                Body.Surface = SurfaceType2;
                Body.Part = static_cast<uint8>(Region);
                Visitor(Body);
            }
        };
        const auto Limb = [&BonePoint, &Sphere](const FName First, const FName Last, const FVector& Start, const FVector& End, const float Radius, const EBBBMonsterHitRegion Region)
        {
            const FVector A = BonePoint(First, Start);
            const FVector B = BonePoint(Last, End);
            const int32 Steps = FMath::Clamp(FMath::CeilToInt(FVector::Distance(A, B) / Radius), 1, 8);
            for (int32 Step = 0; Step <= Steps; ++Step)
            {
                Sphere(FMath::Lerp(A, B, static_cast<float>(Step) / Steps), Radius, Region);
            }
        };
        Sphere(BonePoint(TEXT("pelvis"), FVector(0, 0, -25)), 23.0f, EBBBMonsterHitRegion::Torso);
        Sphere(BonePoint(TEXT("spine_01"), FVector(0, 0, 0)), 25.0f, EBBBMonsterHitRegion::Torso);
        Sphere(BonePoint(TEXT("spine_03"), FVector(0, 0, 30)), 24.0f, EBBBMonsterHitRegion::Torso);
        Sphere(BonePoint(TEXT("head"), FVector(0, 0, 70)), 14.0f, EBBBMonsterHitRegion::Head);
        Limb(TEXT("upperarm_l"), TEXT("lowerarm_l"), FVector(0, -30, 30), FVector(0, -48, 10), 10.0f, EBBBMonsterHitRegion::LeftArm);
        Limb(TEXT("lowerarm_l"), TEXT("hand_l"), FVector(0, -48, 10), FVector(0, -55, -5), 9.0f, EBBBMonsterHitRegion::LeftArm);
        Limb(TEXT("upperarm_r"), TEXT("lowerarm_r"), FVector(0, 30, 30), FVector(0, 48, 10), 10.0f, EBBBMonsterHitRegion::RightArm);
        Limb(TEXT("lowerarm_r"), TEXT("hand_r"), FVector(0, 48, 10), FVector(0, 55, -5), 9.0f, EBBBMonsterHitRegion::RightArm);
        Limb(TEXT("thigh_l"), TEXT("calf_l"), FVector(0, -14, -30), FVector(0, -14, -56), 12.0f, EBBBMonsterHitRegion::LeftLeg);
        Limb(TEXT("calf_l"), TEXT("foot_l"), FVector(0, -14, -56), FVector(0, -14, -80), 10.0f, EBBBMonsterHitRegion::LeftLeg);
        Limb(TEXT("thigh_r"), TEXT("calf_r"), FVector(0, 14, -30), FVector(0, 14, -56), 12.0f, EBBBMonsterHitRegion::RightLeg);
        Limb(TEXT("calf_r"), TEXT("foot_r"), FVector(0, 14, -56), FVector(0, 14, -80), 10.0f, EBBBMonsterHitRegion::RightLeg);
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
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            if (Health[Index].CurrentHealth <= 0.0f)
            {
                continue;
            }

            FBBBMassCollisionBody Body;
            Body.Entity = Chunk.GetEntity(Index);
            Body.Surface = SurfaceType2;
            const FTransform& Transform = Transforms[Index].GetTransform();
            const float Scale = FMath::Clamp(Avoidance[Index].CollisionRadius / 45.0f, 0.5f, 2.0f);
            Body.Center = Transform.GetLocation();
            Body.Radius = 200.0f * Scale;
            Body.bCompound = true;
            Mass->AddCollisionBody(Body);
        }
    });
}
