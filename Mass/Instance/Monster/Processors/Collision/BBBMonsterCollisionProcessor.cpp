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
    EntityQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FBBBMonsterMobilityFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
}

void UBBBMonsterCollisionProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    UBBBMassSubsystem* Mass = Context.GetWorld()->GetSubsystem<UBBBMassSubsystem>();
    Mass->BeginCollisionFrame();
    const float Now = Context.GetWorld()->GetTimeSeconds();
    EntityQuery.ForEachEntityChunk(Context, [Mass, Now](FMassExecutionContext& Chunk)
    {
        const auto Transforms = Chunk.GetFragmentView<FTransformFragment>();
        const auto Avoidance = Chunk.GetFragmentView<FBBBMonsterAvoidanceFragment>();
        const auto Health = Chunk.GetFragmentView<FBBBMonsterHealthFragment>();
        const auto Actors = Chunk.GetFragmentView<FMassActorFragment>();
        const auto Mobility = Chunk.GetFragmentView<FBBBMonsterMobilityFragment>();
        const auto Network = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
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
            const auto* Actor = Cast<ABBBMonsterPresentationActor>(Actors[Index].Get());
            const auto* Mesh = Actor ? Actor->GetMonsterMesh() : nullptr;
            const float Scale = FMath::Clamp(Avoidance[Index].CollisionRadius / 45.0f, 0.5f, 2.0f);
            const auto* Definition = Network[Index].Definition.Get();
            const float CrawlProgress = Mobility[Index].bCrawling
                ? FMath::Clamp((Now - Mobility[Index].CrawlStartedAt) / (Definition ? Definition->CrawlTransitionDuration : 1.0f), 0.0f, 1.0f)
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
            const auto Sphere = [Mass, &Body, Scale](const FVector& Center, const float Radius, const EBBBMonsterHitRegion Region)
            {
                Body.Center = Center;
                Body.Radius = Radius * Scale;
                Body.Part = static_cast<uint8>(Region);
                Mass->AddCollisionBody(Body);
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
    });
}
