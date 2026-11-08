#include "BBBMonsterGoreDefinition.h"

#include "Engine/SkeletalMesh.h"
#include "BBBMonsterPresentationDefinition.h"

UBBBMonsterGoreDefinition::UBBBMonsterGoreDefinition()
{
    Destruction = CreateDefaultSubobject<UBBBMonsterDestructionDefinition>(TEXT("Destruction"));
    Dismemberment = CreateDefaultSubobject<UBBBMonsterDismembermentDefinition>(TEXT("Dismemberment"));
}

const UBBBMonsterBloodPresentationDefinition* UBBBMonsterGoreDefinition::GetBlood() const
{
    const auto* Presentation = GetTypedOuter<UBBBMonsterPresentationDefinition>();
    return Presentation ? Presentation->Blood.Get() : nullptr;
}

bool UBBBMonsterGoreDefinition::IsReady() const
{
    if (!Body || !ProjectionAtlas || !GetBlood() || Materials.Num() != Body->GetMaterials().Num()
        || !Destruction || !Dismemberment || Destruction->Regions.Num() != 8 || !Destruction->Interior
        || Dismemberment->Parts.IsEmpty()
        || Destruction->Interior->GetSkeleton() != Body->GetSkeleton()
        || !FMath::IsFinite(Destruction->HitAccumulation) || Destruction->HitAccumulation < 0.01f
        || Destruction->HitAccumulation > 1 || !FMath::IsFinite(Destruction->OpeningThreshold)
        || Destruction->OpeningThreshold < 0.01f || Destruction->OpeningThreshold > 1)
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
    for (const auto& Region : Destruction->Regions)
    {
        if (Region.Contact.ContainsNaN() || Region.Rotation.ContainsNaN() || Region.Extent.ContainsNaN()
            || Region.Extent.GetMin() < 0.2 || Body->GetRefSkeleton().FindBoneIndex(Region.Bone) == INDEX_NONE)
        {
            return false;
        }
    }
    bool Ids[256] = {};
    uint8 PartMask = 0;
    for (const auto& Part : Dismemberment->Parts)
    {
        const int32 Region = static_cast<int32>(Part.Region);
        if (Part.Id < 1 || Part.Id > 255 || Ids[Part.Id] || Region < 1 || Region > 5
            || Part.Center.ContainsNaN() || Part.Contact.ContainsNaN() || Part.Rotation.ContainsNaN()
            || Part.Extent.ContainsNaN() || Part.Extent.GetMin() < 0.2 || !FMath::IsFinite(Part.BloodStrength)
            || Part.BloodStrength < 0 || Part.BloodStrength > 1
            || Body->GetRefSkeleton().FindBoneIndex(Part.Bone) == INDEX_NONE
            || !FMath::IsFinite(Part.AtlasRect.X) || !FMath::IsFinite(Part.AtlasRect.Y)
            || !FMath::IsFinite(Part.AtlasRect.Z) || !FMath::IsFinite(Part.AtlasRect.W)
            || Part.AtlasRect.X < 0 || Part.AtlasRect.Y < 0 || Part.AtlasRect.Z <= 0 || Part.AtlasRect.W <= 0
            || Part.AtlasRect.X + Part.AtlasRect.Z > 1 || Part.AtlasRect.Y + Part.AtlasRect.W > 1
            || !Part.Tissue || !Part.DetachedSkin || !Part.DetachedMesh
            || (Part.RemainingSkin != nullptr) != (Part.RemainingMesh != nullptr)
            || (Part.RemainingSkin && Part.RemainingSkin->GetSkeleton() != Body->GetSkeleton()))
        {
            return false;
        }
        Ids[Part.Id] = true;
        PartMask |= 1 << Region;
    }
    return PartMask == 0x3e;
}
