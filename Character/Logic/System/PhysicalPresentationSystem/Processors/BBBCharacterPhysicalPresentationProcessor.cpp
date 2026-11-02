#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/Processors/BBBCharacterPhysicalPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/DomainData/Context/BBBCharacterPhysicalPresentationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/Processors/BBBCharacterHitReactionComponent.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysicsEngine/BodyInstance.h"
#include "HitReactProfile.h"

void FBBBCharacterPhysicalPresentationProcessor::Update(FBBBCharacterPhysicalPresentationUpdateContext &Context) const
{
    auto &State = Context.Data.PhysicalPresentation.PhysicalPresentationState;
    const auto &Life = Context.Data.Life.ReadLifeState();
    const auto &Hit = Context.Data.Life.ReadHitState();
    if (!Life.bInitialized)
    {
        return;
    }
    if (State.bRagdoll)
    {
        const auto &Snapshot = State.RagdollSnapshot;
        if (Context.Data.External.ReadNetworkIdentityState().bIsMirror
            && Snapshot.Revision > State.AppliedRagdollRevision)
        {
            // 只还原身体锚点与线速度 四肢仍由本机物理求解 不重演历史冲量
            const FBodyInstance *Pelvis = Context.Mesh.GetBodyInstance(TEXT("pelvis"));
            if (Pelvis)
            {
                const FVector Delta = Snapshot.PelvisLocation - Pelvis->GetUnrealWorldTransform().GetLocation();
                Context.Mesh.AddWorldOffset(Delta, false, nullptr, ETeleportType::TeleportPhysics);
                Context.Mesh.SetAllPhysicsLinearVelocity(Snapshot.Velocity);
                State.AppliedRagdollRevision = Snapshot.Revision;
            }
        }
        return;
    }
    UPhysicsAsset *Asset = Context.Mesh.GetPhysicsAsset();
    if (!ensureMsgf(Asset, TEXT("角色物理表现缺少 PhysicsAsset %s"), *Context.Character.GetName()))
    {
        return;
    }
    if (State.bRagdollRequested || Life.Phase == EBBBCharacterLifePhase::Dead)
    {
        if (!ensureMsgf(Context.Mesh.GetBodyInstance(TEXT("pelvis")),
            TEXT("角色布娃娃缺少 pelvis 刚体 %s"), *Context.Character.GetName()))
        {
            State.bRagdollRequested = false;
            return;
        }
        // 本阶段在 CMC 停止之前执行 先继承已经成立的运动速度
        const bool bRestored = Context.Data.External.ReadNetworkIdentityState().bIsMirror
            && State.RagdollSnapshot.Revision > 0;
        const FVector Velocity = bRestored ? State.RagdollSnapshot.Velocity
            : Context.Character.GetVelocity() + State.PendingImpactVelocity;
        State.PendingImpactVelocity = FVector::ZeroVector;
        Context.HitReaction.ClearSimulation();
        Context.PhysicalAnimation.SetComponentTickEnabled(false);
        // 释放局部受击的目标刚体与驱动约束 由全身物理独立接管
        Context.PhysicalAnimation.SetSkeletalMeshComponent(nullptr);
        // 脱离胶囊后 锚点跟随不会再次搬动物理身体
        Context.Mesh.DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
        // 自由布娃娃不再求值动画与通知 不停止或清空任何蒙太奇槽位
        Context.Mesh.bPauseAnims = true;
        Context.Mesh.SetCollisionProfileName(TEXT("Ragdoll"));
        Context.Mesh.SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
        Context.Mesh.SetAllBodiesSimulatePhysics(true);
        Context.Mesh.SetAllBodiesPhysicsBlendWeight(1.0f);
        Context.Mesh.SetSimulatePhysics(true);
        // 局部受击资产关闭了重力 全身布娃娃必须逐刚体重新开启
        Context.Mesh.SetEnableGravityOnAllBodiesBelow(true, NAME_None);
        Context.Mesh.SetAllPhysicsLinearVelocity(Velocity);
        Context.Mesh.WakeAllRigidBodies();
        State.bRagdoll = true;
        State.bRagdollRequested = false;
        State.bGetUpRequested = false;
        State.bGetUpComplete = false;
        if (!bRestored)
        {
            auto &Snapshot = State.RagdollSnapshot;
            Snapshot.Phase = EBBBCharacterPhysicalPhase::Ragdoll;
            Snapshot.GetUpMontage = nullptr;
            Snapshot.GetUpPosition = 0.0f;
            ++Snapshot.Episode;
            ++Snapshot.Transition;
            ++Snapshot.Revision;
        }
        State.PelvisLocation = Context.Mesh.GetBodyInstance(TEXT("pelvis"))->GetUnrealWorldTransform().GetLocation();
        if (bRestored)
        {
            Context.Mesh.AddWorldOffset(State.RagdollSnapshot.PelvisLocation - State.PelvisLocation,
                false, nullptr, ETeleportType::TeleportPhysics);
            State.PelvisLocation = State.RagdollSnapshot.PelvisLocation;
            State.AppliedRagdollRevision = State.RagdollSnapshot.Revision;
        }
        State.HitSerial = Hit.Serial;
        return;
    }
    if (State.RagdollSnapshot.Phase == EBBBCharacterPhysicalPhase::Recovering
        || Hit.Serial == 0 || Hit.Serial <= State.HitSerial)
    {
        return;
    }
    const double Age = Context.Data.External.ReadWorldState().WorldTimeSeconds - Hit.Time;
    if (Age > 0.25 || Hit.Direction.IsNearlyZero())
    {
        State.HitSerial = Hit.Serial;
        return;
    }

    FName Bone = Hit.Bone;
    while (!Bone.IsNone() && Asset->FindBodyIndex(Bone) == INDEX_NONE)
    {
        Bone = Context.Mesh.GetParentBone(Bone);
    }
    if (Bone.IsNone() || Bone == TEXT("root") || Bone == TEXT("pelvis"))
    {
        Bone = TEXT("spine_02");
    }
    if (!ensureMsgf(Asset->FindBodyIndex(Bone) != INDEX_NONE && !Context.HitReaction.AvailableProfiles.IsEmpty(),
                    TEXT("角色受击缺少刚体或 Profile %s"), *Context.Character.GetName()))
    {
        return;
    }

    FHitReactImpulseParams Impulse;
    Impulse.LinearImpulse.bApplyImpulse = true;
    Impulse.LinearImpulse.Impulse = Life.Phase == EBBBCharacterLifePhase::Downed ? 350.0f : 700.0f;
    Impulse.AngularImpulse.bApplyImpulse = true;
    Impulse.AngularImpulse.Impulse = Life.Phase == EBBBCharacterLifePhase::Downed ? 800.0f : 1600.0f;
    FHitReactImpulse_WorldParams World;
    World.LinearDirection = Hit.Direction;
    World.AngularDirection = FVector::CrossProduct(FVector::UpVector, Hit.Direction).GetSafeNormal();
    const FHitReactInputParams Params(Context.HitReaction.AvailableProfiles[0], Bone, true);
    if (Context.HitReaction.HitReact(Params, Impulse, World))
    {
        Context.PhysicalAnimation.SetComponentTickEnabled(true);
        State.HitSerial = Hit.Serial;
    }
}
