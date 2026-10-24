#include "BBBMonsterGorePresentationComponent.h"

#include "BBBMonsterBloodPresentationSubsystem.h"
#include "BBBMonsterGibPresentationSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterGoreDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterGoreFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"

namespace
{
    FTransform ReferenceBone(const USkeletalMesh& Mesh, const FName Name)
    {
        const auto& Skeleton = Mesh.GetRefSkeleton();
        const int32 Bone = Skeleton.FindBoneIndex(Name);
        if (Bone == INDEX_NONE)
        {
            return FTransform::Identity;
        }
        FTransform Transform = Skeleton.GetRefBonePose()[Bone];
        for (int32 Parent = Skeleton.GetParentIndex(Bone); Parent != INDEX_NONE; Parent = Skeleton.GetParentIndex(Parent))
        {
            Transform *= Skeleton.GetRefBonePose()[Parent];
        }
        return Transform;
    }

    FVector CurrentCenter(const UBBBMonsterGoreDefinition& Definition,
        const FBBBMonsterGoreWoundDefinition& Wound, const USkeletalMeshComponent& Body)
    {
        if (Body.GetBoneIndex(Wound.Bone) != INDEX_NONE)
        {
            return Body.GetSocketTransform(Wound.Bone).TransformPosition(
                ReferenceBone(*Definition.Body, Wound.Bone).InverseTransformPosition(Wound.Contact));
        }
        return Body.GetComponentTransform().TransformPosition(Wound.Contact);
    }

    double SurfaceDistance(const UBBBMonsterGoreDefinition& Definition,
        const FBBBMonsterGoreWoundDefinition& Wound, const USkeletalMeshComponent& Body,
        const FBBBMonsterHitReactionFragment& Hit, FVector& Projected)
    {
        if (Body.GetBoneIndex(Wound.Bone) == INDEX_NONE)
        {
            return TNumericLimits<double>::Max();
        }
        const FTransform Reference = ReferenceBone(*Definition.Body, Wound.Bone);
        const FTransform Current = Body.GetSocketTransform(Wound.Bone);
        const FVector Position = Reference.TransformPosition(Current.InverseTransformPosition(Hit.Position));
        const FVector Direction = Reference.TransformVectorNoScale(
            Current.InverseTransformVectorNoScale(Hit.Direction)).GetSafeNormal();
        const FVector Normal = Wound.Rotation.RotateVector(FVector::UpVector);
        const double Facing = FVector::DotProduct(Direction, Normal);
        if (Facing > -0.25)
        {
            return TNumericLimits<double>::Max();
        }
        // 物理碰撞面不等于衣物表面 沿真实弹道投到配对创口平面
        const double Travel = FVector::DotProduct(Wound.Contact - Position, Normal) / Facing;
        if (Travel < -Definition.HitRadius || Travel > Wound.Extent.Z * 2 + Definition.HitRadius)
        {
            return TNumericLimits<double>::Max();
        }
        Projected = Position + Direction * Travel;
        const FVector Local = Wound.Rotation.UnrotateVector(Projected - Wound.Contact);
        return FMath::Square(Local.X / Wound.Extent.X) + FMath::Square(Local.Y / Wound.Extent.Y);
    }
}

UBBBMonsterGorePresentationComponent::UBBBMonsterGorePresentationComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UBBBMonsterGorePresentationComponent::UpdateWounds(FBBBMonsterGoreFragment& Result,
    const UBBBMonsterGoreDefinition& Definition, const uint8 DestroyedParts,
    const FBBBMonsterHitReactionFragment& Hit, USkeletalMeshComponent* Mesh, const float Now, const bool bAuthority)
{
    if (bAuthority)
    {
        for (int32 Region = 1; Region < 6; ++Region)
        {
            auto& Slot = Result.Wounds[Region - 1];
            if ((DestroyedParts & (1u << Region)) == 0 || Slot.PresetId != 0)
            {
                continue;
            }
            const FBBBMonsterGoreWoundDefinition* Best = nullptr;
            double Distance = TNumericLimits<double>::Max();
            for (const auto& Wound : Definition.Wounds)
            {
                if (!Wound.bPermanent || static_cast<int32>(Wound.Region) != Region)
                {
                    continue;
                }
                const bool bContact = Mesh && Hit.Age <= 0.25f && static_cast<int32>(Hit.Region) == Region;
                const double Candidate = bContact && Definition.Body
                    ? FVector::DistSquared(CurrentCenter(Definition, Wound, *Mesh), Hit.Position) : Wound.Id;
                if (Candidate < Distance)
                {
                    Best = &Wound;
                    Distance = Candidate;
                }
            }
            if (Best)
            {
                Slot.PresetId = static_cast<uint8>(Best->Id);
                Slot.Strength = 1;
                Slot.Center = Best->Center;
                Slot.Orientation = Best->Rotation.Quaternion();
                Slot.RadiusDepth = FVector2D(Best->Extent.X, Best->Extent.Z);
            }
        }
    }

    if (Hit.Serial == Result.ConsumedSerial || Hit.Age > 0.25f)
    {
        return;
    }
    Result.ConsumedSerial = Hit.Serial;
    const int32 Region = static_cast<int32>(Hit.Region);
    if (!Mesh || !Definition.Body || Region < 0 || Region >= 6
        || (Region > 0 && (DestroyedParts & (1u << Region)) != 0) || Now < Result.NextWoundTime
        || !FMath::IsFinite(Definition.HitRadius) || Definition.HitRadius < 0.2f)
    {
        return;
    }
    Result.NextWoundTime = Now + 1 / FMath::Clamp(Definition.WoundUpdatesPerSecond, 1.0f, 30.0f);

    const FBBBMonsterGoreWoundDefinition* Surface = nullptr;
    for (const auto& Wound : Definition.Wounds)
    {
        if (!Wound.bPermanent && !Wound.Tissue && Wound.Cutout == 0
            && (!Surface || Wound.Id < Surface->Id))
        {
            Surface = &Wound;
        }
    }
    if (!Surface)
    {
        return;
    }

    FName Anchor = Mesh->FindClosestBone(Hit.Position);
    if (Anchor.IsNone())
    {
        return;
    }
    const FTransform Reference = ReferenceBone(*Definition.Body, Anchor);
    const FTransform Current = Mesh->GetSocketTransform(Anchor);
    FVector Center = Reference.TransformPosition(Current.InverseTransformPosition(Hit.Position));
    FVector Normal = Reference.TransformVectorNoScale(
        Current.InverseTransformVectorNoScale(Hit.Normal)).GetSafeNormal();
    if (Center.ContainsNaN() || Normal.IsNearlyZero())
    {
        return;
    }
    const FBBBMonsterGoreWoundDefinition* Deep = nullptr;
    double Distance = 1;
    for (const auto& Wound : Definition.Wounds)
    {
        if (Wound.bPermanent || Wound.Region != Hit.Region || !Wound.Tissue)
        {
            continue;
        }
        FVector Projected;
        const double Candidate = SurfaceDistance(Definition, Wound, *Mesh, Hit, Projected);
        if (Candidate < Distance)
        {
            Deep = &Wound;
            Distance = Candidate;
            Center = Projected;
            Normal = Wound.Rotation.RotateVector(FVector::UpVector);
        }
    }
    int32 SelectedSlot = INDEX_NONE;
    for (int32 Index = 5; Index < 7; ++Index)
    {
        const auto* Existing = Definition.FindWound(Result.Wounds[Index].PresetId);
        const bool bInsideTissue = Existing && Deep && Existing->Id == Deep->Id;
        if (Existing && (bInsideTissue
            || FVector::DistSquared(Result.Wounds[Index].Center, Center) < FMath::Square(Definition.HitRadius * 2)))
        {
            SelectedSlot = Index;
            break;
        }
    }
    if (SelectedSlot == INDEX_NONE)
    {
        SelectedSlot = 5 + Result.NextSurfaceSlot;
        Result.NextSurfaceSlot = (Result.NextSurfaceSlot + 1) % 2;
        Result.Wounds[SelectedSlot] = FBBBMonsterGoreWound();
    }
    auto& Slot = Result.Wounds[SelectedSlot];
    const auto* Existing = Definition.FindWound(Slot.PresetId);
    if (Existing && Existing->Tissue && !Existing->bPermanent)
    {
        Slot.Strength = FMath::Min(1.0f, Slot.Strength + 0.35f);
        return;
    }
    Slot.PresetId = static_cast<uint8>(Surface->Id);
    Slot.Center = Center;
    Slot.Orientation = FRotationMatrix::MakeFromZ(Normal).ToQuat();
    // 逻辑碰撞面可能在外皮之外 以参考骨骼平面限制投影深度
    const float ProjectionDepth = FMath::Max(Definition.HitRadius,
        static_cast<float>(FMath::Abs(FVector::DotProduct(Center - Reference.GetLocation(), Normal))));
    Slot.RadiusDepth = FVector2D(Definition.HitRadius, ProjectionDepth);
    Slot.Strength = FMath::Min(1.0f, Slot.Strength + 0.35f);
    if (Region == 0 && (DestroyedParts & 1u) != 0)
    {
        Slot.Strength = 1;
    }

    // 组织模型仅在其预制适配范围内选择 不把任意命中强制移到固定伤口
    if (Deep && Slot.Strength >= 0.65f)
    {
        Slot.PresetId = static_cast<uint8>(Deep->Id);
        // 缓存配对资源的坐标范围 后续命中按同一创口归并
        Slot.Center = Deep->Center;
        Slot.Orientation = Deep->Rotation.Quaternion();
        Slot.RadiusDepth = FVector2D(Deep->Extent.X, Deep->Extent.Z);
        Slot.Strength = 1;
    }
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
    const FVector& Velocity, const uint32 Seed, const bool bRestore, UBBBMonsterSoundPresentationDefinition* Sounds)
{
    if (!Prepare(Definition))
    {
        return;
    }
    bool bDamaged = false;
    for (const auto& Wound : Facts.Wounds)
    {
        bDamaged |= Wound.PresetId != 0 && Wound.Strength > 0;
    }
    if (bUsingDamageMaterials != bDamaged)
    {
        for (int32 Index = 0; Index < Definition.Materials.Num(); ++Index)
        {
            Body->SetMaterial(Index, bDamaged ? Definition.Materials[Index] : OriginalMaterials[Index]);
        }
        bUsingDamageMaterials = bDamaged;
    }

    const auto Write = [this](const int32 Index, const float Value)
    {
        const auto& Data = Body->GetCustomPrimitiveData().Data;
        if (!FMath::IsNearlyEqual(Data.IsValidIndex(Index) ? Data[Index] : 0.0f, Value))
        {
            Body->SetCustomPrimitiveDataFloat(Index, Value);
        }
    };
    TArray<FBBBMonsterBloodImpact, TInlineAllocator<5>> Bursts;
    for (int32 Index = 0; Index < 7; ++Index)
    {
        const auto& State = Facts.Wounds[Index];
        const auto* Preset = Definition.FindWound(State.PresetId);
        const bool bActive = Preset && State.Strength > 0;
        Write(1 + Index, bActive ? State.PresetId : 0);
        Write(10 + Index, bActive ? State.Strength : 0);
        if (Index >= 5)
        {
            const float Values[] = {
                static_cast<float>(State.Center.X), static_cast<float>(State.Center.Y), static_cast<float>(State.Center.Z),
                static_cast<float>(State.Orientation.X), static_cast<float>(State.Orientation.Y),
                static_cast<float>(State.Orientation.Z), static_cast<float>(State.Orientation.W),
                static_cast<float>(State.RadiusDepth.X), static_cast<float>(State.RadiusDepth.Y)};
            for (int32 Part = 0; Part < 9; ++Part)
            {
                Write(17 + (Index - 5) * 9 + Part, Values[Part]);
            }
        }

        const bool bTissueVisible = bActive && Preset->Tissue && Body->GetPredictedLODLevel() <= 2;
        if (bTissueVisible && !Tissue[Index])
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
            const auto& PrimitiveData = Body->GetCustomPrimitiveData().Data;
            Tissue[Index]->SetCustomPrimitiveDataFloat(8, PrimitiveData.IsValidIndex(8) ? PrimitiveData[8] : 0);
            if (bTissueVisible)
            {
                if (Tissue[Index]->GetSkeletalMeshAsset() != Preset->Tissue)
                {
                    Tissue[Index]->SetSkeletalMesh(Preset->Tissue);
                    Tissue[Index]->SetLeaderPoseComponent(Body);
                }
            }
            Tissue[Index]->SetVisibility(bTissueVisible);
        }
        if (bActive && Preset->bPermanent && AppliedPresets[Index] != State.PresetId)
        {
            if (!bRestore && Hit.Age <= 0.25f && Preset->DetachedSkin && Preset->DetachedMesh)
            {
                if (auto* Pieces = GetWorld()->GetSubsystem<UBBBMonsterGibPresentationSubsystem>())
                {
                    Pieces->Emit(*Body, *Preset->DetachedMesh, *Preset->DetachedSkin, Preset->Bone,
                        Hit.Direction, Velocity, Seed ^ Hit.Serial, Sounds);
                }
                FBBBMonsterBloodImpact Burst;
                Burst.Position = CurrentCenter(Definition, *Preset, *Body);
                Burst.Direction = Hit.Direction;
                Burst.Normal = Hit.Normal;
                Burst.Region = Preset->Region;
                Burst.Seed = Seed ^ Hit.Serial ^ Index * 7919u;
                Burst.bSevering = true;
                Burst.Source = ResidueSource;
                Bursts.Add(Burst);
            }
            if (Preset->DetachedSkin && Body->IsPhysicsStateCreated() && Body->bHasValidBodies)
            {
                Body->SetAllBodiesBelowPhysicsDisabled(Preset->Bone, true);
            }
        }
        AppliedPresets[Index] = bActive ? State.PresetId : 0;
    }
    if (!Bursts.IsEmpty())
    {
        if (auto* Blood = GetWorld()->GetSubsystem<UBBBMonsterBloodPresentationSubsystem>())
        {
            Blood->Publish(*Definition.Blood, Bursts);
        }
    }
}

void UBBBMonsterGorePresentationComponent::ResetPresentation()
{
    if (Body)
    {
        if (AppliedDefinition)
        {
            for (const auto& Wound : AppliedDefinition->Wounds)
            {
                if (Wound.bPermanent && Wound.DetachedSkin && Body->IsPhysicsStateCreated() && Body->bHasValidBodies)
                {
                    Body->SetAllBodiesBelowPhysicsDisabled(Wound.Bone, false);
                }
            }
        }
        for (int32 Index = 0; Index < OriginalMaterials.Num(); ++Index)
        {
            Body->SetMaterial(Index, OriginalMaterials[Index]);
        }
        for (int32 Index = 1; Index < 36; ++Index)
        {
            Body->SetCustomPrimitiveDataFloat(Index, 0);
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
