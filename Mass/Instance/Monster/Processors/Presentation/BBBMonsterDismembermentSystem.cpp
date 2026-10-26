#include "BBBMonsterDismembermentSystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterGoreDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterGoreFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"

void FBBBMonsterDismembermentSystem::Resolve(FBBBMonsterGoreFragment& Result,
    const UBBBMonsterGoreDefinition& Definition, const uint8 DestroyedParts,
    const FBBBMonsterHitReactionFragment& Hit, const USkeletalMeshComponent* Mesh)
{
    if (!Definition.Dismemberment)
    {
        return;
    }
    for (int32 Region = 1; Region < 6; ++Region)
    {
        auto& Slot = Result.DismembermentPresets[Region - 1];
        if ((DestroyedParts & (1u << Region)) == 0 || Slot != 0)
        {
            continue;
        }
        const FBBBMonsterDismembermentPartDefinition* Best = nullptr;
        double Distance = TNumericLimits<double>::Max();
        for (const auto& Part : Definition.Dismemberment->Parts)
        {
            if (static_cast<int32>(Part.Region) != Region)
            {
                continue;
            }
            double Candidate = Part.Id;
            const bool bContact = Mesh && Definition.Body && Hit.Age <= 0.25f && static_cast<int32>(Hit.Region) == Region;
            const int32 Bone = Definition.Body ? Definition.Body->GetRefSkeleton().FindBoneIndex(Part.Bone) : INDEX_NONE;
            if (bContact && Bone != INDEX_NONE && Mesh->GetBoneIndex(Part.Bone) != INDEX_NONE)
            {
                const auto& Skeleton = Definition.Body->GetRefSkeleton();
                FTransform Reference = Skeleton.GetRefBonePose()[Bone];
                for (int32 Parent = Skeleton.GetParentIndex(Bone); Parent != INDEX_NONE; Parent = Skeleton.GetParentIndex(Parent))
                {
                    Reference *= Skeleton.GetRefBonePose()[Parent];
                }
                const FVector Center = Mesh->GetSocketTransform(Part.Bone).TransformPosition(Reference.InverseTransformPosition(Part.Contact));
                Candidate = FVector::DistSquared(Center, Hit.Position);
            }
            if (Candidate < Distance)
            {
                Distance = Candidate;
                Best = &Part;
            }
        }
        if (Best)
        {
            Slot = static_cast<uint8>(Best->Id);
        }
    }
}
