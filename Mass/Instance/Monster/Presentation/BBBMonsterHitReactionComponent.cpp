#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterHitReactionComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "HitReactProfile.h"
#include "HitReactStatics.h"
#include "IAnimationBudgetAllocator.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysicsEngine/PhysicsConstraintTemplate.h"
#include "PhysicsEngine/BodyInstance.h"
#include "Engine/SkeletalMesh.h"
#include "SkeletalMeshComponentBudgeted.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterPresentationDefinition.h"

UBBBMonsterHitReactionComponent::UBBBMonsterHitReactionComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bUseFixedSimulationRate = false;
    Cooldown = 0.0f;
    bAutoActivate = false;
}

void UBBBMonsterHitReactionComponent::ConfigurePresentation(const UBBBMonsterPresentationDefinition* Settings)
{
    if (PresentationSettings == Settings)
    {
        return;
    }
    Deactivate();
    ResetPresentation();
    PresentationSettings = Settings;
    AvailableProfiles.Reset();
    if (Settings)
    {
        AvailableProfiles = {Settings->HitReaction.TorsoProfile.Get(), Settings->HitReaction.HeadProfile.Get(),
            Settings->HitReaction.ArmsProfile.Get(), Settings->HitReaction.LegsProfile.Get()};
    }
    if (Settings && GetWorld() && GetWorld()->IsGameWorld())
    {
        Activate(true);
    }
}

void UBBBMonsterHitReactionComponent::ApplyHitFacts(const FBBBMonsterHitReactionFragment& Hit, const bool bAlive)
{
    if (!bAlive)
    {
        bPresentationAlive = false;
        ObservedHitSerial = Hit.Serial;
        return;
    }
    bPresentationAlive = bAlive;

    if (Hit.Serial == 0 || Hit.Serial == ObservedHitSerial)
    {
        return;
    }

    if (!PresentationSettings || !bHasInitialized || !bProfilesLoaded)
    {
        return;
    }

    if (!CanHitReact() || Hit.Age > 0.2f || Hit.Direction.ContainsNaN() || Hit.Direction.IsNearlyZero() || Hit.Position.ContainsNaN())
    {
        UE_LOG(LogTemp, Verbose, TEXT("[BBBHitReact]跳过命中编号=%u LOD=%d 命中年龄=%.3f 可受击=%d"),
            Hit.Serial, IsValid(Mesh) ? Mesh->GetPredictedLODLevel() : INDEX_NONE, Hit.Age, CanHitReact());
        ObservedHitSerial = Hit.Serial;
        return;
    }

    ObservedHitSerial = Hit.Serial;
    const UPhysicsAsset* PhysicsAsset = Mesh->GetPhysicsAsset();
    if (!ensureMsgf(PhysicsAsset, TEXT("[BBBHitReact]表现网格缺少物理资产 %s"), *GetOwner()->GetName()))
    {
        return;
    }

    int32 ProfileIndex = 0;
    const auto& Settings = PresentationSettings->HitReaction;
    const auto& Region = Settings.ReadRegion(Hit.Region);
    const FName BoneName = Region.Bone;
    const float LinearStrength = Region.LinearImpulse;
    const float AngularStrength = Region.AngularImpulse;
    switch (Hit.Region)
    {
        case EBBBMonsterHitRegion::Head:
            ProfileIndex = 1;
            break;
        case EBBBMonsterHitRegion::LeftArm:
        case EBBBMonsterHitRegion::RightArm:
            ProfileIndex = 2;
            break;
        case EBBBMonsterHitRegion::LeftLeg:
        case EBBBMonsterHitRegion::RightLeg:
            ProfileIndex = 3;
            break;
        default:
            break;
    }

    if (Mesh->IsBoneHiddenByName(BoneName))
    {
        return;
    }
    if (!ensureMsgf(PhysicsAsset->FindBodyIndex(BoneName) != INDEX_NONE && AvailableProfiles.IsValidIndex(ProfileIndex),
        TEXT("[BBBHitReact]受击部位缺少物理刚体或配置 %s %s"), *GetOwner()->GetName(), *BoneName.ToString()))
    {
        return;
    }

    const FVector Direction = Hit.Direction.GetSafeNormal();
    const FVector Lever = Hit.Position - Mesh->GetBoneLocation(BoneName);
    FVector AngularAxis = FVector::CrossProduct(Lever, Direction).GetSafeNormal();
    if (Region.RotationMode == EBBBMonsterHitRotationMode::BodyDeflection)
    {
        AngularAxis = FVector::CrossProduct(FVector::UpVector, Direction).GetSafeNormal();
    }
    if (Region.RotationMode == EBBBMonsterHitRotationMode::LimbDeflection || AngularAxis.IsNearlyZero())
    {
        AngularAxis = FVector::CrossProduct(Direction, FVector::UpVector).GetSafeNormal();
    }

    FHitReactImpulseParams Impulse;
    Impulse.LinearImpulse.bApplyImpulse = true;
    Impulse.LinearImpulse.Impulse = LinearStrength;
    Impulse.AngularImpulse.bApplyImpulse = !AngularAxis.IsNearlyZero();
    Impulse.AngularImpulse.Impulse = AngularStrength;
    FHitReactImpulse_WorldParams World;
    World.LinearDirection = Direction;
    World.AngularDirection = AngularAxis;
    const FHitReactInputParams Params(AvailableProfiles[ProfileIndex], BoneName, true);
    RequestAnimationUpdate();
    const auto* PreviousBlend = PhysicsBlends.FindByPredicate([BoneName](const auto& Blend)
    {
        return Blend.SimulatedBoneName == BoneName && Blend.PhysicsState.IsActive();
    });
    const bool bRepeating = PreviousBlend != nullptr;
    float PreviousAlpha = 0;
    if (PreviousBlend)
    {
        const auto& State = PreviousBlend->PhysicsState;
        PreviousAlpha = State.GetBlendStateAlpha();
        if (State.GetBlendState() == EHitReactBlendState::BlendHold)
        {
            PreviousAlpha = 1.0f;
        }
        if (State.GetBlendState() == EHitReactBlendState::BlendOut)
        {
            PreviousAlpha = 1.0f - PreviousAlpha;
        }
    }
    const float Scale = FMath::Clamp(Settings.ImpulseScale * (bRepeating ? Settings.RepeatImpulseScale : 1.0f),
        0.0f, Settings.MaximumImpulseScale);
    if (HitReact(Params, Impulse, World, Scale))
    {
        // 连射保留当前局部混合 同骨骼只保留一个状态 冲量按连射配置缩放
        int32 KeepIndex = INDEX_NONE;
        float MinimumAge = MAX_flt;
        for (int32 Index = 0; Index < PhysicsBlends.Num(); ++Index)
        {
            const auto& Blend = PhysicsBlends[Index];
            if (Blend.SimulatedBoneName == BoneName && Blend.PhysicsState.GetElapsedTime() <= MinimumAge)
            {
                MinimumAge = Blend.PhysicsState.GetElapsedTime();
                KeepIndex = Index;
            }
        }
        if (bRepeating && PhysicsBlends.IsValidIndex(KeepIndex))
        {
            auto& State = PhysicsBlends[KeepIndex].PhysicsState;
            float Low = 0;
            float High = 1;
            for (int32 Iteration = 0; Iteration < 12; ++Iteration)
            {
                const float Middle = (Low + High) * .5f;
                if (State.Params.BlendIn.Ease(Middle) < PreviousAlpha)
                {
                    Low = Middle;
                }
                else
                {
                    High = Middle;
                }
            }
            float NormalizedTime = (Low + High) * .5f;
            if (PreviousAlpha >= 1.0f - SMALL_NUMBER)
            {
                NormalizedTime = 1.0f;
            }
            State.SetElapsedTime(State.Params.BlendIn.BlendTime * NormalizedTime);
        }
        for (int32 Index = PhysicsBlends.Num() - 1; Index >= 0; --Index)
        {
            if (Index != KeepIndex && PhysicsBlends[Index].SimulatedBoneName == BoneName)
            {
                PhysicsBlends.RemoveAt(Index);
            }
        }
        if (!bRepeating)
        {
            PendingAnchorRoot = BoneName;
        }
        bHasAppliedReaction = true;
        if (IsValid(PhysicalAnimation))
        {
            PhysicalAnimation->AddTickPrerequisiteComponent(this);
            PhysicalAnimation->SetComponentTickEnabled(true);
        }
        UE_LOG(LogTemp, Verbose, TEXT("[BBBHitReact]命中编号=%u 部位=%s LOD=%d 直线=%.1f 旋转=%.1f 力度=%.2f"),
            Hit.Serial, *BoneName.ToString(), Mesh->GetPredictedLODLevel(), LinearStrength, AngularStrength, Scale);
    }
}

bool UBBBMonsterHitReactionComponent::CanHitReact_Implementation() const
{
    return bPresentationAlive
        && IsValid(GetOwner()) && !GetOwner()->IsHidden() && IsValid(Mesh)
        && Mesh->IsVisible() && Mesh->GetPredictedLODLevel() <= 2;
}

void UBBBMonsterHitReactionComponent::RequestAnimationUpdate() const
{
    auto* BudgetMesh = Cast<USkeletalMeshComponentBudgeted>(Mesh);
    auto* Budget = BudgetMesh && GetWorld() ? IAnimationBudgetAllocator::Get(GetWorld()) : nullptr;
    if (Budget)
    {
        Budget->ForceNextTickThisFrame(BudgetMesh);
    }
}

void UBBBMonsterHitReactionComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    if (!CanHitReact())
    {
        ResetHitReactSystem();
        return;
    }

    UpdateJointFrames();
    const bool bWasReacting = !PhysicsBlends.IsEmpty();
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (!PhysicsBlends.IsEmpty())
    {
        RequestAnimationUpdate();
    }
    if (PhysicsBlends.IsEmpty() && (bWasReacting || bHasAppliedReaction || bCollisionEnabledChanged
        || bPhysicalAnimationProfileChanged || bConstraintProfileChanged))
    {
        ResetHitReactSystem();
    }
}

void UBBBMonsterHitReactionComponent::UpdateJointFrames()
{
    if (!IsValid(Mesh) || !Mesh->GetPhysicsAsset() || (PendingAnchorRoot.IsNone() && PhysicsBlends.IsEmpty()))
    {
        return;
    }
    const auto LocalPose = Mesh->GetBoneSpaceTransformsView();
    const FReferenceSkeleton& Skeleton = Mesh->GetSkeletalMeshAsset()->GetRefSkeleton();
    if (LocalPose.Num() < Skeleton.GetNum())
    {
        return;
    }
    TArray<FTransform, TInlineAllocator<128>> AnimatedPose;
    AnimatedPose.Append(LocalPose.GetData(), Skeleton.GetNum());
    for (int32 Index = 1; Index < AnimatedPose.Num(); ++Index)
    {
        AnimatedPose[Index] *= AnimatedPose[Skeleton.GetParentIndex(Index)];
    }
    const UPhysicsAsset* Asset = Mesh->GetPhysicsAsset();
    for (int32 Index = 0; Index < Asset->ConstraintSetup.Num(); ++Index)
    {
        FConstraintInstance* Joint = Mesh->GetConstraintInstanceByIndex(Index);
        if (!Joint)
        {
            continue;
        }
        const int32 ChildIndex = Skeleton.FindBoneIndex(Joint->ConstraintBone1);
        const int32 ParentIndex = Skeleton.FindBoneIndex(Joint->ConstraintBone2);
        if (ChildIndex == INDEX_NONE || ParentIndex == INDEX_NONE)
        {
            continue;
        }
        const auto AffectsChild = [&Skeleton, ChildIndex](const FName Root)
        {
            const int32 RootIndex = Skeleton.FindBoneIndex(Root);
            return RootIndex != INDEX_NONE && (ChildIndex == RootIndex || Skeleton.BoneIsChildOf(ChildIndex, RootIndex));
        };
        const bool bAffected = AffectsChild(PendingAnchorRoot) || PhysicsBlends.ContainsByPredicate([&AffectsChild](const auto& Blend)
        {
            return AffectsChild(Blend.SimulatedBoneName);
        });
        if (!bAffected)
        {
            continue;
        }
        // 局部物理相对当前动画活动 避免把弯腰和挥臂本身当作关节越限
        FTransform ChildFrame = Asset->ConstraintSetup[Index]->DefaultInstance.GetRefFrame(EConstraintFrame::Frame1);
        ChildFrame.SetLocation(FVector::ZeroVector);
        FTransform ParentFrame = (ChildFrame * AnimatedPose[ChildIndex]).GetRelativeTransform(AnimatedPose[ParentIndex]);
        ParentFrame.SetScale3D(FVector::OneVector);
        Joint->SetRefFrame(EConstraintFrame::Frame1, ChildFrame);
        Joint->SetRefFrame(EConstraintFrame::Frame2, ParentFrame);
    }
    PendingAnchorRoot = NAME_None;
}

void UBBBMonsterHitReactionComponent::ResetHitReactSystem()
{
    if (IsValid(PhysicalAnimation))
    {
        PhysicalAnimation->SetComponentTickEnabled(false);
    }
    PendingAnchorRoot = NAME_None;
    const bool bHadReaction = bHasAppliedReaction || !PhysicsBlends.IsEmpty() || PendingImpulse.IsValid() || bCollisionEnabledChanged || bPhysicalAnimationProfileChanged || bConstraintProfileChanged;
    if (bHadReaction && IsValid(Mesh))
    {
        Mesh->GetBoneSpaceTransformsView();
        Mesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
        Mesh->SetAllPhysicsAngularVelocityInRadians(FVector::ZeroVector);
        Mesh->SetAllBodiesPhysicsBlendWeight(0.0f);
        Mesh->SetAllBodiesSimulatePhysics(false);
        if (const UPhysicsAsset* Asset = Mesh->GetPhysicsAsset())
        {
            for (int32 Index = 0; Index < Asset->ConstraintSetup.Num(); ++Index)
            {
                if (FConstraintInstance* Joint = Mesh->GetConstraintInstanceByIndex(Index))
                {
                    const auto& Definition = Asset->ConstraintSetup[Index]->DefaultInstance;
                    Joint->SetRefFrame(EConstraintFrame::Frame1, Definition.GetRefFrame(EConstraintFrame::Frame1));
                    Joint->SetRefFrame(EConstraintFrame::Frame2, Definition.GetRefFrame(EConstraintFrame::Frame2));
                }
            }
        }
        Mesh->SetConstraintProfileForAll(NAME_None);
        if (IsValid(PhysicalAnimation))
        {
            PhysicalAnimation->SetSkeletalMeshComponent(nullptr);
            PhysicalAnimation->SetSkeletalMeshComponent(Mesh);
        }
        if (bCollisionEnabledChanged)
        {
            Mesh->SetCollisionEnabled(DefaultCollisionEnabled);
        }
        UHitReactStatics::FinalizeMeshPhysics(Mesh);
        if (Mesh->IsRegistered() && Mesh->GetSkeletalMeshAsset())
        {
            // 停用物理结束阶段后立即发布当前动画姿势 避免读缓冲残留上一帧
            Mesh->RefreshBoneTransforms();
        }
    }

    PhysicsBlends.Reset();
    SmoothedBoneWeights.Reset();
    PendingImpulse = {};
    LastProfileHitReactTimes.Reset();
    LastHitReactTime = -1.0f;
    bHasAppliedReaction = false;
    bCollisionEnabledChanged = false;
    bPhysicalAnimationProfileChanged = false;
    bConstraintProfileChanged = false;
    SleepHitReact();
}

void UBBBMonsterHitReactionComponent::ResetPresentation()
{
    ResetHitReactSystem();
    ObservedHitSerial = 0;
    bPresentationAlive = true;
}

uint32 UBBBMonsterHitReactionComponent::GetObservedHitSerial() const
{
    return ObservedHitSerial;
}

void UBBBMonsterHitReactionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ResetHitReactSystem();
    CancelAsyncLoading();
    Super::EndPlay(EndPlayReason);
}
