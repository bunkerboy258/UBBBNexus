#include "BBBCharacterFallProcessor.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/DomainData/Context/BBBCharacterPhysicalPresentationUpdateContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicsEngine/BodyInstance.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysicsEngine/SkeletalBodySetup.h"

void FBBBCharacterFallProcessor::Update(FBBBCharacterPhysicalPresentationUpdateContext &Context) const
{
    auto &State = Context.Data.PhysicalPresentation.PhysicalPresentationState;
    const auto &Life = Context.Data.Life.ReadLifeState();
    const auto &Config = Context.Character.GetCharacterConfig();
    auto *Movement = Context.Character.GetCharacterMovement();
    if (Context.Data.External.ReadNetworkIdentityState().bIsMirror || !Life.bInitialized || !Movement)
    {
        State.FallContacts.Reset();
        return;
    }
    if (!ensureMsgf(FMath::IsFinite(Config.FallRagdollDistance) && Config.FallRagdollDistance > 0
        && FMath::IsFinite(Config.FallSafeSpeed) && Config.FallSafeSpeed >= 0
        && FMath::IsFinite(Config.FallFullDamageSpeed) && Config.FallFullDamageSpeed > Config.FallSafeSpeed
        && FMath::IsFinite(Config.FallMaximumDamage) && Config.FallMaximumDamage >= 0,
        TEXT("角色跌落配置无效 距离必须为正数 满额速度必须大于无伤速度")))
    {
        State.FallContacts.Reset();
        State.bTrackingFall = false;
        return;
    }
    if (State.bFallRagdoll && !State.bRagdoll && !State.bRagdollRequested)
    {
        for (const auto &Entry : State.FallBodyDamping)
        {
            if (FBodyInstance *Body = Context.Mesh.GetBodyInstance(Entry.Key))
            {
                Body->LinearDamping = Entry.Value;
                Body->UpdateDampingProperties();
            }
        }
        State.FallBodyDamping.Reset();
        State.bFallRagdoll = false;
        State.bFallLanded = false;
        State.FallBodySpeeds.Reset();
    }
    if (State.bFallRagdoll && State.bRagdoll && !State.bFallLanded)
    {
        for (const auto &Contact : State.FallContacts)
        {
            const FName Bone = Contact.Hit.MyBoneName;
            if (Contact.Episode != State.RagdollSnapshot.Episode || !Contact.Hit.bBlockingHit
                || !State.FallBodySpeeds.Contains(Bone)
                || Contact.Hit.ImpactNormal.Z < FMath::Cos(FMath::DegreesToRadians(Config.Locomotion.WalkableFloorAngle)))
            {
                continue;
            }
            State.bFallLanded = true;
            const float Alpha = FMath::Clamp((Contact.DownSpeed - Config.FallSafeSpeed)
                / (Config.FallFullDamageSpeed - Config.FallSafeSpeed), 0.0f, 1.0f);
            const float Damage = Config.FallMaximumDamage * FMath::SmoothStep(0.0f, 1.0f, Alpha);
            if (Damage > 0)
            {
                FBBBHitContribution Hit;
                Hit.AddDamage = Damage;
                Hit.Bone = Bone;
                Hit.Position = Contact.Hit.ImpactPoint;
                Hit.Normal = Contact.Hit.ImpactNormal;
                Context.Character.SubmitInput(FBBBCharacterDamageLocalControlPacket{{Hit}});
            }
            // 伤害经下一帧生命系统消费后才允许起身
            State.ImpactReleaseAt = FMath::Max(State.ImpactReleaseAt,
                Context.Data.External.ReadWorldState().WorldTimeSeconds + 0.1);
            break;
        }
        State.FallBodySpeeds.Reset();
        for (FName Bone : {FName(TEXT("pelvis")), FName(TEXT("spine_01")), FName(TEXT("spine_02")), FName(TEXT("spine_03"))})
        {
            if (const FBodyInstance *Body = Context.Mesh.GetBodyInstance(Bone))
            {
                State.FallBodySpeeds.Add(Bone, FMath::Max(0.0f, float(-Body->GetUnrealWorldVelocity().Z)));
            }
        }
    }
    State.FallContacts.Reset();
    if (State.bFallRagdoll && State.bFallLanded && Life.Phase == EBBBCharacterLifePhase::Alive)
    {
        State.bGetUpRequested = true;
    }
    if (State.bRagdoll || State.bRagdollRequested || Life.Phase != EBBBCharacterLifePhase::Alive
        || State.RagdollSnapshot.Phase != EBBBCharacterPhysicalPhase::Animated || !Movement->IsFalling()
        || Movement->bJustTeleported)
    {
        State.bTrackingFall = false;
        return;
    }
    const double Height = Context.Character.GetActorLocation().Z;
    if (!State.bTrackingFall)
    {
        State.FallPeakHeight = Height;
        State.bTrackingFall = true;
    }
    State.FallPeakHeight = FMath::Max(State.FallPeakHeight, Height);
    if (Movement->Velocity.Z < 0 && State.FallPeakHeight - Height >= Config.FallRagdollDistance)
    {
        // 高处自由落体不沿用局部受击的强制动 起身后恢复原值
        if (const UPhysicsAsset *Asset = Context.Mesh.GetPhysicsAsset())
        {
            for (const USkeletalBodySetup *Setup : Asset->SkeletalBodySetups)
            {
                if (Setup)
                {
                    if (FBodyInstance *Body = Context.Mesh.GetBodyInstance(Setup->BoneName))
                    {
                        State.FallBodyDamping.Add(Setup->BoneName, Body->LinearDamping);
                        Body->LinearDamping = 0.01f;
                        Body->UpdateDampingProperties();
                    }
                }
            }
        }
        State.bFallRagdoll = true;
        State.bFallLanded = false;
        State.bRagdollRequested = true;
        State.bGetUpRequested = false;
        State.bGetUpComplete = false;
        State.bTrackingFall = false;
        for (FName Bone : {FName(TEXT("pelvis")), FName(TEXT("spine_01")), FName(TEXT("spine_02")), FName(TEXT("spine_03"))})
        {
            State.FallBodySpeeds.Add(Bone, float(-Movement->Velocity.Z));
        }
    }
}
