#include "BBBMonsterSeveringPresentationComponent.h"

#include "BBBMonsterBloodPresentationSubsystem.h"
#include "BBBMonsterGibPresentationSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSoundPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

UBBBMonsterSeveringPresentationComponent::UBBBMonsterSeveringPresentationComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UBBBMonsterSeveringPresentationComponent::ApplyWoundFacts(const FBBBMonsterHealthFragment& Health,
    const UBBBMonsterDefinition& Definition, const FBBBMonsterHitReactionFragment& Hit, const uint32 Seed, const bool bNewActor)
{
    auto* Mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
    if (!Mesh)
    {
        return;
    }
    if (AppliedStages.Num() != 6)
    {
        AppliedStages.Init(0.0f, 6);
    }
    bool bDamaged = Health.DestroyedParts != 0;
    bool bNeedsGeometry = (Health.DestroyedParts & 0x3eu) != 0;
    bool bStagesChanged = false;
    for (uint8 Region = 0; Region < 6; ++Region)
    {
        const float Ratio = static_cast<float>(Health.PartDamageRatios.Get(static_cast<EBBBMonsterHitRegion>(Region)));
        const bool bDestroyed = (Health.DestroyedParts & (1u << Region)) != 0;
        const float Stage = bDestroyed || Ratio >= 1.0f ? 1.0f : Ratio >= 0.6f ? 0.66f : Ratio >= 0.2f ? 0.33f : 0.0f;
        bDamaged |= Stage > 0.0f;
        bNeedsGeometry |= Region > 0 && Stage >= 1.0f;
        if (AppliedStages[Region] != Stage)
        {
            Mesh->SetCustomPrimitiveDataFloat(Region + 1, Stage);
            AppliedStages[Region] = Stage;
            bStagesChanged = true;
        }
    }
    if (!bDamaged)
    {
        return;
    }
    const bool bShowGeometry = Mesh->GetPredictedLODLevel() <= 1;
    if (!bStagesChanged && AppliedParts == Health.DestroyedParts && !bNewActor)
    {
        if (Wounds)
        {
            Wounds->SetVisibility(bShowGeometry);
        }
        if (!bNeedsGeometry || !bShowGeometry || Wounds)
        {
            return;
        }
    }
    if (!ensureMsgf(Definition.WoundGeometry && Definition.WoundMaterials.Num() == Mesh->GetNumMaterials(),
        TEXT("[BBBZombieWound]资源配置不完整 Actor=%s"), *GetOwner()->GetName()))
    {
        return;
    }
    if (IntactMaterials.IsEmpty())
    {
        for (int32 Index = 0; Index < Mesh->GetNumMaterials(); ++Index)
        {
            IntactMaterials.Add(Mesh->GetMaterial(Index));
            Mesh->SetMaterial(Index, Definition.WoundMaterials[Index]);
        }
    }
    Mesh->SetCustomPrimitiveDataFloat(7, static_cast<float>(Seed % 4));
    if (!bNeedsGeometry)
    {
        return;
    }
    if (!Wounds && bShowGeometry)
    {
        bStagesChanged = true;
        Wounds = NewObject<USkeletalMeshComponent>(GetOwner());
        Wounds->PrimaryComponentTick.bCanEverTick = false;
        Wounds->SetCastShadow(false);
        Wounds->SetRenderInDepthPass(false);
        Wounds->SetVisibleInRayTracing(false);
        Wounds->SetReceivesDecals(false);
        Wounds->SetAllowAnimCurveEvaluation(false);
        Wounds->SetDisablePostProcessBlueprint(true);
        Wounds->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Wounds->SetCanEverAffectNavigation(false);
        Wounds->bUseAttachParentBound = true;
        Wounds->SetupAttachment(Mesh);
        Wounds->RegisterComponent();
    }
    if (Wounds && Wounds->GetSkeletalMeshAsset() != Definition.WoundGeometry)
    {
        Wounds->SetSkeletalMesh(Definition.WoundGeometry);
    }
    if (Wounds && Wounds->LeaderPoseComponent.Get() != Mesh)
    {
        Wounds->SetLeaderPoseComponent(Mesh);
    }
    if (Wounds)
    {
        Wounds->SetVisibility(bShowGeometry);
        for (int32 Region = 0; Region < 6 && bStagesChanged; ++Region)
        {
            Wounds->SetCustomPrimitiveDataFloat(Region + 1, AppliedStages[Region]);
        }
        Wounds->SetCustomPrimitiveDataFloat(7, static_cast<float>(Seed % 4));
    }

    TArray<FBBBMonsterBloodImpact, TInlineAllocator<5>> Bursts;
    bool bPlaySever = false;
    for (const auto& Part : Definition.SeveredParts)
    {
        const uint8 Bit = 1u << static_cast<uint8>(Part.Region);
        if ((Health.DestroyedParts & Bit) == 0 || (AppliedParts & Bit) != 0)
        {
            continue;
        }
        if (!ensureMsgf(Part.DetachedMesh && Part.PoseMesh && Mesh->GetBoneIndex(Part.Bone) != INDEX_NONE,
            TEXT("[BBBZombieWound]缺少封闭姿态部件 Bone=%s"), *Part.Bone.ToString()))
        {
            continue;
        }
        if (!bNewActor && Hit.Age <= 0.25f)
        {
            GetWorld()->GetSubsystem<UBBBMonsterGibPresentationSubsystem>()->Emit(
                *Mesh, *Part.DetachedMesh, *Part.PoseMesh, Part.Bone, Hit.Direction, Definition.SoundPresentation);
            FBBBMonsterBloodImpact Burst;
            Burst.Position = Mesh->GetSocketLocation(Part.Bone);
            Burst.Direction = Hit.Direction;
            Burst.Normal = Hit.Normal;
            Burst.Region = Part.Region;
            Burst.Seed = Seed ^ (static_cast<uint32>(Part.Region) * 7919u) ^ Hit.Serial;
            Burst.bSevering = true;
            Bursts.Add(Burst);
            bPlaySever = true;
        }
        Mesh->SetAllBodiesBelowPhysicsDisabled(Part.Bone, true);
        AppliedParts |= Bit;
    }
    if (!Bursts.IsEmpty() && Definition.BloodPresentation)
    {
        GetWorld()->GetSubsystem<UBBBMonsterBloodPresentationSubsystem>()->Publish(*Definition.BloodPresentation, Bursts);
    }
    if (bPlaySever && Definition.SoundPresentation && !Definition.SoundPresentation->Severings.IsEmpty())
    {
        const auto* Audio = Definition.SoundPresentation.Get();
        bool bAudible = false;
        for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            if (const auto* Listener = It->Get(); Listener && Listener->IsLocalController())
            {
                FVector Eye;
                FRotator Rotation;
                Listener->GetPlayerViewPoint(Eye, Rotation);
                bAudible |= FVector::DistSquared(Eye, Mesh->GetComponentLocation()) <= FMath::Square(Audio->AudibleDistance);
            }
        }
        if (bAudible)
        {
            UGameplayStatics::PlaySoundAtLocation(this, Audio->Severings[Seed % Audio->Severings.Num()], Mesh->GetComponentLocation(),
                0.8f, 0.96f + static_cast<float>(Seed % 9) * 0.01f, 0.0f, Audio->Attenuation, Audio->ActionConcurrency);
        }
    }
}

void UBBBMonsterSeveringPresentationComponent::ResetPresentation()
{
    if (Wounds)
    {
        Wounds->SetVisibility(false);
    }
    auto* Mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
    if (Mesh)
    {
        for (int32 Index = 0; Index < IntactMaterials.Num(); ++Index)
        {
            Mesh->SetMaterial(Index, IntactMaterials[Index]);
        }
        for (int32 Region = 1; Region <= 7; ++Region)
        {
            Mesh->SetCustomPrimitiveDataFloat(Region, 0.0f);
        }
        const FName Bones[] = {TEXT("head"), TEXT("upperarm_l"), TEXT("upperarm_r"), TEXT("thigh_l"), TEXT("thigh_r")};
        for (uint8 Index = 0; Index < UE_ARRAY_COUNT(Bones); ++Index)
        {
            if ((AppliedParts & (1u << (Index + 1))) != 0)
            {
                Mesh->SetAllBodiesBelowPhysicsDisabled(Bones[Index], false);
            }
        }
    }
    IntactMaterials.Reset();
    AppliedStages.Reset();
    AppliedParts = 0;
}

void UBBBMonsterSeveringPresentationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ResetPresentation();
    if (Wounds)
    {
        Wounds->DestroyComponent();
        Wounds = nullptr;
    }
    Super::EndPlay(EndPlayReason);
}
