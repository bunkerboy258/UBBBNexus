#include "BBBMonsterGorePresentationComponent.h"
#include "BBBMonsterBloodPresentationSubsystem.h"
#include "BBBMonsterGibPresentationSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterGoreDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterGoreFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"

namespace
{
    FVector CurrentContact(const UBBBMonsterGoreDefinition& Definition,
        const FBBBMonsterDismembermentPartDefinition& Part, const USkeletalMeshComponent& Body)
    {
        const auto& Skeleton = Definition.Body->GetRefSkeleton();
        const int32 Bone = Skeleton.FindBoneIndex(Part.Bone);
        if (Bone == INDEX_NONE || Body.GetBoneIndex(Part.Bone) == INDEX_NONE)
        {
            return Body.GetComponentTransform().TransformPosition(Part.Contact);
        }
        FTransform Reference = Skeleton.GetRefBonePose()[Bone];
        for (int32 Parent = Skeleton.GetParentIndex(Bone); Parent != INDEX_NONE; Parent = Skeleton.GetParentIndex(Parent))
        {
            Reference *= Skeleton.GetRefBonePose()[Parent];
        }
        return Body.GetSocketTransform(Part.Bone).TransformPosition(Reference.InverseTransformPosition(Part.Contact));
    }

    void Write(USkeletalMeshComponent& Mesh, const int32 Index, const float Value)
    {
        const auto& Data = Mesh.GetCustomPrimitiveData().Data;
        if (!FMath::IsNearlyEqual(Data.IsValidIndex(Index) ? Data[Index] : 0.0f, Value))
        {
            Mesh.SetCustomPrimitiveDataFloat(Index, Value);
        }
    }
}

UBBBMonsterGorePresentationComponent::UBBBMonsterGorePresentationComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UBBBMonsterGorePresentationComponent::Prepare(const UBBBMonsterGoreDefinition& Definition)
{
    if (!Body)
    {
        Body = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
    }
    if (!Body)
    {
        return false;
    }
    if (AppliedDefinition == &Definition && Body->GetSkeletalMeshAsset() == Definition.Body)
    {
        return true;
    }
    if (!Definition.IsReady())
    {
        return false;
    }
    ResetPresentation();
    Body->SetSkeletalMesh(Definition.Body);
    Body->EmptyOverrideMaterials();
    for (int32 Index = 0; Index < Definition.Materials.Num(); ++Index)
    {
        OriginalMaterials.Add(Body->GetMaterial(Index));
    }
    AppliedDefinition = &Definition;
    return true;
}

void UBBBMonsterGorePresentationComponent::Apply(const UBBBMonsterGoreDefinition& Definition,
    const FBBBMonsterGoreFragment& Facts, const FBBBMonsterHitReactionFragment& Hit,
    const FVector& Velocity, const uint32 Seed, const bool bRestore)
{
    if (!Prepare(Definition))
    {
        return;
    }
    bool bDamaged = false;
    bool bOpenTorso = false;
    bool bNewTorsoOpening = false;
    for (int32 Index = 0; Index < 5; ++Index)
    {
        bDamaged |= Facts.DismembermentPresets[Index] != 0;
        Write(*Body, 1 + Index, Facts.DismembermentPresets[Index]);
    }
    for (int32 Index = 0; Index < 8; ++Index)
    {
        const float Strength = Facts.DestructionStrengths[Index] / 255.0f;
        const auto& Data = Body->GetCustomPrimitiveData().Data;
        const float Previous = Data.IsValidIndex(10 + Index) ? Data[10 + Index] : 0.0f;
        bNewTorsoOpening |= Previous < Definition.Destruction->OpeningThreshold
            && Strength >= Definition.Destruction->OpeningThreshold;
        bDamaged |= Strength > 0;
        bOpenTorso |= Strength >= Definition.Destruction->OpeningThreshold;
        Write(*Body, 10 + Index, Strength);
    }
    if (bUsingDamageMaterials != bDamaged)
    {
        for (int32 Index = 0; Index < Definition.Materials.Num(); ++Index)
        {
            Body->SetMaterial(Index, bDamaged ? Definition.Materials[Index] : OriginalMaterials[Index]);
        }
        bUsingDamageMaterials = bDamaged;
    }
    TArray<FBBBMonsterBloodImpact, TInlineAllocator<5>> Bursts;
    if (bNewTorsoOpening && !bRestore && Hit.Serial != 0 && Hit.Age >= 0.0f && Hit.Age <= 0.25f
        && Hit.Region == EBBBMonsterHitRegion::Torso)
    {
        FBBBMonsterBloodImpact Burst;
        Burst.Position = Hit.Position;
        Burst.Direction = Hit.Direction;
        Burst.Normal = Hit.Normal;
        Burst.Region = Hit.Region;
        Burst.Seed = Seed ^ Hit.Serial;
        Burst.bWoundOpening = true;
        Burst.Source = ResidueSource;
        Bursts.Add(Burst);
    }
    for (int32 Index = 0; Index < 6; ++Index)
    {
        const auto* Part = Index < 5 ? Definition.Dismemberment->Find(Facts.DismembermentPresets[Index]) : nullptr;
        USkeletalMesh* Resource = Part ? Part->Tissue.Get() : nullptr;
        if (Index == 5 && bOpenTorso)
        {
            Resource = Definition.Destruction->Interior;
        }
        const bool bVisible = Resource && Body->GetPredictedLODLevel() <= 2;
        if (bVisible && !Tissue[Index])
        {
            Tissue[Index] = NewObject<USkeletalMeshComponent>(GetOwner());
            Tissue[Index]->PrimaryComponentTick.bCanEverTick = false;
            Tissue[Index]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            Tissue[Index]->SetCanEverAffectNavigation(false);
            Tissue[Index]->SetReceivesDecals(false);
            Tissue[Index]->SetAllowAnimCurveEvaluation(false);
            Tissue[Index]->SetDisablePostProcessBlueprint(true);
            Tissue[Index]->bUseAttachParentBound = true;
            Tissue[Index]->SetupAttachment(Body);
            Tissue[Index]->RegisterComponent();
            Tissue[Index]->SetComponentTickEnabled(false);
        }
        if (Tissue[Index])
        {
            if (bVisible && Tissue[Index]->GetSkeletalMeshAsset() != Resource)
            {
                Tissue[Index]->SetSkeletalMesh(Resource);
                Tissue[Index]->SetLeaderPoseComponent(Body);
            }
            if (bVisible)
            {
                Tissue[Index]->SetForcedLOD(FMath::Clamp(Body->GetPredictedLODLevel() + 1, 1, Resource->GetLODNum()));
            }
            const auto& Data = Body->GetCustomPrimitiveData().Data;
            for (int32 Parameter = 1; Parameter < 18; ++Parameter)
            {
                Write(*Tissue[Index], Parameter, Data.IsValidIndex(Parameter) ? Data[Parameter] : 0);
            }
            Tissue[Index]->SetVisibility(bVisible);
        }
        if (Part && AppliedPresets[Index] != Part->Id)
        {
            if (!bRestore && Hit.Age <= 0.25f && Part->DetachedSkin && Part->DetachedMesh)
            {
                if (auto* Pieces = GetWorld()->GetSubsystem<UBBBMonsterGibPresentationSubsystem>())
                {
                    Pieces->Emit(*Body, *Part->DetachedMesh, *Part->DetachedSkin, Part->Bone,
                        Hit.Direction, Velocity, Seed ^ Hit.Serial, *Definition.GetTypedOuter<UBBBMonsterPresentationDefinition>());
                }
                FBBBMonsterBloodImpact Burst;
                Burst.Position = CurrentContact(Definition, *Part, *Body);
                Burst.Direction = Hit.Direction;
                Burst.Normal = Hit.Normal;
                Burst.Region = Part->Region;
                Burst.Seed = Seed ^ Hit.Serial ^ Index * 7919u;
                Burst.bSevering = true;
                Burst.Source = ResidueSource;
                Bursts.Add(Burst);
            }
            if (Part->DetachedSkin && Body->IsPhysicsStateCreated() && Body->bHasValidBodies)
            {
                Body->SetAllBodiesBelowPhysicsDisabled(Part->Bone, true);
            }
        }
        if (Index < 5)
        {
            AppliedPresets[Index] = Part ? static_cast<uint8>(Part->Id) : 0;
        }
    }
    if (!Bursts.IsEmpty())
    {
        if (auto* Blood = GetWorld()->GetSubsystem<UBBBMonsterBloodPresentationSubsystem>())
        {
            Blood->Publish(*Definition.GetBlood(), Bursts);
        }
    }
}

void UBBBMonsterGorePresentationComponent::ResetPresentation()
{
    if (Body)
    {
        if (AppliedDefinition && AppliedDefinition->Dismemberment)
        {
            for (const auto& Part : AppliedDefinition->Dismemberment->Parts)
            {
                if (Body->IsPhysicsStateCreated() && Body->bHasValidBodies)
                {
                    Body->SetAllBodiesBelowPhysicsDisabled(Part.Bone, false);
                }
            }
        }
        for (int32 Index = 0; Index < OriginalMaterials.Num(); ++Index)
        {
            Body->SetMaterial(Index, OriginalMaterials[Index]);
        }
        for (int32 Index = 1; Index < 18; ++Index)
        {
            Write(*Body, Index, 0);
        }
    }
    for (auto& Component : Tissue)
    {
        if (Component)
        {
            Component->SetVisibility(false);
        }
    }
    OriginalMaterials.Reset();
    AppliedDefinition = nullptr;
    FMemory::Memzero(AppliedPresets);
    bUsingDamageMaterials = false;
}

void UBBBMonsterGorePresentationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ResetPresentation();
    for (auto& Component : Tissue)
    {
        if (Component)
        {
            Component->DestroyComponent();
            Component = nullptr;
        }
    }
    Body = nullptr;
    Super::EndPlay(EndPlayReason);
}
