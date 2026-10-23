#include "BBBMonsterGoreDefinition.h"

#include "Engine/SkeletalMesh.h"

const FBBBMonsterGoreWoundDefinition* UBBBMonsterGoreDefinition::FindWound(const uint8 Id) const
{
    return Wounds.FindByPredicate([Id](const FBBBMonsterGoreWoundDefinition& Wound)
    {
        return Wound.Id == Id;
    });
}

bool UBBBMonsterGoreDefinition::IsReady() const
{
    if (!Body || !ProjectionAtlas || !Blood || Materials.Num() != Body->GetMaterials().Num() || Wounds.IsEmpty())
    {
        return false;
    }

    for (const auto& Material : Materials)
    {
        if (!Material)
        {
            return false;
        }
    }

    bool Ids[256] = {};
    for (const auto& Wound : Wounds)
    {
        if (Wound.Id < 1 || Wound.Id > 255 || Ids[Wound.Id] || Wound.Center.ContainsNaN() || Wound.Contact.ContainsNaN()
            || Wound.Rotation.ContainsNaN() || Wound.Extent.ContainsNaN()
            || Wound.Extent.GetMin() < 0.2 || !FMath::IsFinite(Wound.Cutout)
            || !FMath::IsFinite(Wound.BloodStrength)
            || Wound.Cutout < 0 || Wound.Cutout > 1 || Wound.BloodStrength < 0 || Wound.BloodStrength > 1
            || Wound.Bone.IsNone()
            || Body->GetRefSkeleton().FindBoneIndex(Wound.Bone) == INDEX_NONE
            || !FMath::IsFinite(Wound.AtlasRect.X) || !FMath::IsFinite(Wound.AtlasRect.Y)
            || !FMath::IsFinite(Wound.AtlasRect.Z) || !FMath::IsFinite(Wound.AtlasRect.W)
            || Wound.AtlasRect.X < 0 || Wound.AtlasRect.Y < 0 || Wound.AtlasRect.Z <= 0 || Wound.AtlasRect.W <= 0
            || Wound.AtlasRect.X + Wound.AtlasRect.Z > 1 || Wound.AtlasRect.Y + Wound.AtlasRect.W > 1
            || (Wound.Cutout > 0 && !Wound.Tissue)
            || (Wound.DetachedSkin && !Wound.DetachedMesh)
            || (Wound.DetachedMesh && !Wound.DetachedSkin))
        {
            return false;
        }
        Ids[Wound.Id] = true;
    }

    return true;
}
