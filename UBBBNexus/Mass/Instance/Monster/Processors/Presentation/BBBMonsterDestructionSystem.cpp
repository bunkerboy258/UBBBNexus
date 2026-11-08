#include "BBBMonsterDestructionSystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterGoreDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterGoreFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "MassActorSubsystem.h"
#include "MassCommonFragments.h"
#include "MassEntitySubsystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"

int32 FBBBMonsterDestructionSystem::Select(const UBBBMonsterGoreDefinition& Definition,
    const FVector& Position, const FVector& Direction, const FTransform& MeshTransform, const USkeletalMeshComponent* Mesh)
{
    if (!Definition.Body || !Definition.Destruction || Definition.Destruction->Regions.Num() != 8
        || Position.ContainsNaN() || Direction.ContainsNaN() || Direction.IsNearlyZero())
    {
        return INDEX_NONE;
    }
    int32 Selected = INDEX_NONE;
    double Best = TNumericLimits<double>::Max();
    const auto& Skeleton = Definition.Body->GetRefSkeleton();
    for (int32 Index = 0; Index < 8; ++Index)
    {
        const auto& Region = Definition.Destruction->Regions[Index];
        FVector Point = MeshTransform.InverseTransformPosition(Position);
        FVector Travel = MeshTransform.InverseTransformVectorNoScale(Direction).GetSafeNormal();
        const int32 Bone = Skeleton.FindBoneIndex(Region.Bone);
        if (Mesh && Bone != INDEX_NONE && Mesh->GetBoneIndex(Region.Bone) != INDEX_NONE)
        {
            FTransform Reference = Skeleton.GetRefBonePose()[Bone];
            for (int32 Parent = Skeleton.GetParentIndex(Bone); Parent != INDEX_NONE; Parent = Skeleton.GetParentIndex(Parent))
            {
                Reference *= Skeleton.GetRefBonePose()[Parent];
            }
            const FTransform Current = Mesh->GetSocketTransform(Region.Bone);
            Point = Reference.TransformPosition(Current.InverseTransformPosition(Position));
            Travel = Reference.TransformVectorNoScale(Current.InverseTransformVectorNoScale(Direction)).GetSafeNormal();
        }
        const FVector Normal = Region.Rotation.RotateVector(FVector::UpVector);
        const double Facing = FVector::DotProduct(Travel, Normal);
        if (Facing >= -0.05 || Region.Extent.GetMin() <= 0)
        {
            continue;
        }
        const double Distance = FVector::DotProduct(Region.Contact - Point, Normal) / Facing;
        if (Distance < -Region.Extent.Z || Distance > Region.Extent.Z * 4)
        {
            continue;
        }
        const FVector Local = Region.Rotation.UnrotateVector(Point + Travel * Distance - Region.Contact);
        const double Score = FMath::Square(Local.X / Region.Extent.X) + FMath::Square(Local.Y / Region.Extent.Y);
        // 重叠处只选离中心最近的一区 角度相同时按固定下标归属
        if (Score < Best)
        {
            Best = Score;
            Selected = Index;
        }
    }
    return Selected;
}

bool FBBBMonsterDestructionSystem::Query(UWorld& World, FMassEntityHandle Entity,
    const FVector& Position, const FVector& Direction, const float Strength, FBBBMonsterDestructionContribution& Result)
{
    const auto* Entities = World.GetSubsystem<UMassEntitySubsystem>();
    if (!IsInGameThread() || !Entities || !FMath::IsFinite(Strength) || Strength <= 0)
    {
        return false;
    }
    const auto& Manager = Entities->GetEntityManager();
    if (!Manager.IsEntityValid(Entity))
    {
        return false;
    }
    const auto* Network = Manager.GetFragmentDataPtr<FBBBMonsterNetworkFragment>(Entity);
    const auto* Transform = Manager.GetFragmentDataPtr<FTransformFragment>(Entity);
    const auto* Settings = Network && Network->Definition.IsValid() ? Network->Definition->Presentation.Get() : nullptr;
    if (!Settings || !Settings->Gore || !Settings->Gore->Destruction || !Transform)
    {
        return false;
    }
    const auto* ActorFragment = Manager.GetFragmentDataPtr<FMassActorFragment>(Entity);
    const auto* Actor = ActorFragment ? ActorFragment->Get() : nullptr;
    const auto* Mesh = Actor ? Actor->FindComponentByClass<USkeletalMeshComponent>() : nullptr;
    const int32 Region = Select(*Settings->Gore, Position, Direction,
        Settings->MeshTransform * Transform->GetTransform(), Mesh);
    if (Region == INDEX_NONE)
    {
        return false;
    }
    Result.Add(Region, Strength * Settings->Gore->Destruction->HitAccumulation);
    return true;
}

void FBBBMonsterDestructionSystem::Resolve(const FBBBMonsterDamageFragment& Damage, FBBBMonsterGoreFragment& Result)
{
    for (int32 Index = 0; Index < 8; ++Index)
    {
        int32 Total = 0;
        for (const auto& Pair : Damage.Contributions)
        {
            Total = FMath::Min(255, Total + Pair.Value.Destruction.Regions[Index]);
        }
        Result.DestructionStrengths[Index] = static_cast<uint8>(Total);
    }
}
