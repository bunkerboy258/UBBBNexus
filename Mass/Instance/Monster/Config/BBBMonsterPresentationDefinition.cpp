#include "BBBMonsterPresentationDefinition.h"

#include "BBBMonsterBloodPresentationDefinition.h"
#include "BBBMonsterGoreDefinition.h"
#include "BBBMonsterSoundPresentationDefinition.h"
#include "HitReactProfile.h"
#include "Engine/SkeletalMesh.h"
#include "PhysicsEngine/PhysicsAsset.h"

FBBBMonsterHitPresentation::FBBBMonsterHitPresentation()
{
    Head = {TEXT("head"), 540, 2800, EBBBMonsterHitRotationMode::BodyDeflection};
    LeftArm = {TEXT("upperarm_l"), 1080, 3640, EBBBMonsterHitRotationMode::ContactTorque};
    RightArm = {TEXT("upperarm_r"), 1080, 3640, EBBBMonsterHitRotationMode::ContactTorque};
    LeftLeg = {TEXT("thigh_l"), 468, 1960, EBBBMonsterHitRotationMode::LimbDeflection};
    RightLeg = {TEXT("thigh_r"), 468, 1960, EBBBMonsterHitRotationMode::LimbDeflection};
}

const FBBBMonsterHitRegionPresentation& FBBBMonsterHitPresentation::ReadRegion(const EBBBMonsterHitRegion Region) const
{
    switch (Region)
    {
        case EBBBMonsterHitRegion::Head:
            return Head;
        case EBBBMonsterHitRegion::LeftArm:
            return LeftArm;
        case EBBBMonsterHitRegion::RightArm:
            return RightArm;
        case EBBBMonsterHitRegion::LeftLeg:
            return LeftLeg;
        case EBBBMonsterHitRegion::RightLeg:
            return RightLeg;
        default:
            return Torso;
    }
}

FBBBMonsterGibPresentation::FBBBMonsterGibPresentation()
{
    Parts = {
        {TEXT("head"), 420, 160, 340, 4.5f},
        {TEXT("upperarm_l"), 340, 130, 260, 3.5f},
        {TEXT("upperarm_r"), 340, 130, 260, 3.5f},
        {TEXT("lowerarm_l"), 380, 120, 310, 1.6f},
        {TEXT("lowerarm_r"), 380, 120, 310, 1.6f},
        {TEXT("calf_l"), 260, 100, 180, 5},
        {TEXT("calf_r"), 260, 100, 180, 5}};
}

const FBBBMonsterGibPartPresentation& FBBBMonsterGibPresentation::ReadPart(const FName Bone) const
{
    for (const auto& Part : Parts)
    {
        if (Part.Bone == Bone)
        {
            return Part;
        }
    }
    return DefaultPart;
}

UBBBMonsterPresentationDefinition::UBBBMonsterPresentationDefinition()
{
    Gore = CreateDefaultSubobject<UBBBMonsterGoreDefinition>(TEXT("Wounds"));
    Blood = CreateDefaultSubobject<UBBBMonsterBloodPresentationDefinition>(TEXT("Blood"));
    Sounds = CreateDefaultSubobject<UBBBMonsterSoundPresentationDefinition>(TEXT("Sounds"));
}

bool UBBBMonsterPresentationDefinition::IsValid() const
{
    if (!Gore || !Gore->IsReady() || Gore->GetOuter() != this || !Blood || Blood->GetOuter() != this
        || !Blood->HitSpray.IsValid() || !Blood->SeveringSpray.IsValid()
        || !Sounds || Sounds->GetOuter() != this || !Sounds->IsValid() || !PhysicsAsset || !AnimationClass
        || MeshTransform.ContainsNaN() || HitReaction.ImpulseScale < 0 || !FMath::IsFinite(HitReaction.ImpulseScale)
        || HitReaction.ImpulseScale > 4 || !FMath::IsFinite(HitReaction.RepeatImpulseScale)
        || HitReaction.RepeatImpulseScale < 0 || HitReaction.RepeatImpulseScale > 2
        || !FMath::IsFinite(HitReaction.MaximumImpulseScale) || HitReaction.MaximumImpulseScale <= 0
        || HitReaction.MaximumImpulseScale > 4 || !HitReaction.TorsoProfile || !HitReaction.HeadProfile
        || !HitReaction.ArmsProfile || !HitReaction.LegsProfile)
    {
        return false;
    }
    for (const UHitReactProfile* Profile : {HitReaction.TorsoProfile.Get(), HitReaction.HeadProfile.Get(),
        HitReaction.ArmsProfile.Get(), HitReaction.LegsProfile.Get()})
    {
        if (Profile->GetOuter() != this || !FMath::IsFinite(Profile->MaxBlendWeight)
            || Profile->MaxBlendWeight < 0 || Profile->MaxBlendWeight > 1
            || !FMath::IsFinite(Profile->BoneBlendRate) || Profile->BoneBlendRate <= 0
            || Profile->MaxActiveBlends < 1 || Profile->MaxActiveBlends > 6
            || Profile->SubsequentImpulseScalars.Num() > 8 || Profile->BoneOverrides.Num() > 32
            || Profile->RemapSimulatedBones.Num() > 16)
        {
            return false;
        }
    }
    for (int32 Index = 0; Index < 6; ++Index)
    {
        const auto& Region = HitReaction.ReadRegion(static_cast<EBBBMonsterHitRegion>(Index));
        if (Gore->Body->GetRefSkeleton().FindBoneIndex(Region.Bone) == INDEX_NONE
            || PhysicsAsset->FindBodyIndex(Region.Bone) == INDEX_NONE
            || !FMath::IsFinite(Region.LinearImpulse) || Region.LinearImpulse < 0 || Region.LinearImpulse > 10000
            || !FMath::IsFinite(Region.AngularImpulse) || Region.AngularImpulse < 0 || Region.AngularImpulse > 20000)
        {
            return false;
        }
    }
    const auto ValidCycle = [](const FVector& Value)
    {
        return !Value.ContainsNaN() && Value.GetMin() > 0.01;
    };
    if (Animation.MovementCycleSeconds.Num() != 3 || !ValidCycle(Animation.IdleCycleSeconds)
        || !ValidCycle(Animation.SlowWalkCycleSeconds) || !ValidCycle(Animation.PursuitWalkCycleSeconds)
        || Animation.LocomotionWeights.ContainsNaN() || Animation.LocomotionWeights.GetMin() < 0
        || Animation.LocomotionWeights.X + Animation.LocomotionWeights.Y + Animation.LocomotionWeights.Z <= 0
        || !FMath::IsFinite(Animation.AnimationStrideScale) || Animation.AnimationStrideScale <= 0
        || !FMath::IsFinite(Animation.SlowWalkSampleSpeed) || Animation.SlowWalkSampleSpeed <= 0
        || !FMath::IsFinite(Animation.AlertCycleSeconds) || Animation.AlertCycleSeconds <= 0.01
        || !FMath::IsFinite(Animation.DeathAnimationDuration) || Animation.DeathAnimationDuration <= 0
        || !FMath::IsFinite(Animation.CorpseSimulationDuration) || Animation.CorpseSimulationDuration < 0.5
        || Animation.CorpseSimulationDuration > 10 || !FMath::IsFinite(Animation.AnimationHitFraction)
        || Animation.AnimationHitFraction <= 0 || Animation.AnimationHitFraction >= 1)
    {
        return false;
    }
    for (const auto& Cycle : Animation.MovementCycleSeconds)
    {
        if (!ValidCycle(Cycle))
        {
            return false;
        }
    }
    const auto ValidPart = [](const FBBBMonsterGibPartPresentation& Part)
    {
        return FMath::IsFinite(Part.Speed) && Part.Speed >= 0 && Part.Speed <= 2000
            && FMath::IsFinite(Part.Lift) && Part.Lift >= 0 && Part.Lift <= 1000
            && FMath::IsFinite(Part.Spin) && Part.Spin >= 0 && Part.Spin <= 1500
            && FMath::IsFinite(Part.Mass) && Part.Mass >= 0.1f && Part.Mass <= 30;
    };
    if (Gibs.Parts.Num() > 16 || !ValidPart(Gibs.DefaultPart) || !FMath::IsFinite(Gibs.Lifetime)
        || Gibs.Lifetime < 1 || Gibs.Lifetime > 120 || !FMath::IsFinite(Gibs.FadeDuration)
        || Gibs.FadeDuration < 0.1f || Gibs.FadeDuration > 10 || Gibs.SpeedScale.ContainsNaN()
        || Gibs.SpeedScale.GetMin() < 0 || Gibs.SpeedScale.GetMax() > 2
        || !FMath::IsFinite(Gibs.SideSpeed) || Gibs.SideSpeed < 0 || Gibs.SideSpeed > 300
        || !FMath::IsFinite(Gibs.SpinDirectionSpread) || Gibs.SpinDirectionSpread < 0 || Gibs.SpinDirectionSpread > 1
        || !FMath::IsFinite(Gibs.InheritedVelocityScale) || Gibs.InheritedVelocityScale < 0 || Gibs.InheritedVelocityScale > 2
        || !FMath::IsFinite(Gibs.MaximumInheritedSpeed) || Gibs.MaximumInheritedSpeed < 0 || Gibs.MaximumInheritedSpeed > 1500
        || !FMath::IsFinite(Gibs.RestLinearSpeed) || Gibs.RestLinearSpeed < 1 || Gibs.RestLinearSpeed > 30
        || !FMath::IsFinite(Gibs.RestAngularSpeed) || Gibs.RestAngularSpeed < 1 || Gibs.RestAngularSpeed > 30
        || !FMath::IsFinite(Gibs.RestDuration) || Gibs.RestDuration < 0.1f || Gibs.RestDuration > 2)
    {
        return false;
    }
    TSet<FName> Bones;
    for (const auto& Part : Gibs.Parts)
    {
        if (!ValidPart(Part) || Part.Bone.IsNone() || Bones.Contains(Part.Bone))
        {
            return false;
        }
        Bones.Add(Part.Bone);
    }
    return true;
}
