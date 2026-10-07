#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/Processors/BBBCharacterPhysicalPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/DomainData/Context/BBBCharacterPhysicalPresentationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/Processors/BBBCharacterHitReactionComponent.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "HitReactProfile.h"

void FBBBCharacterPhysicalPresentationProcessor::Update(FBBBCharacterPhysicalPresentationUpdateContext &Context) const
{
    auto &State = Context.Data.PhysicalPresentation.PhysicalPresentationState;
    const auto &Life = Context.Data.Life.ReadLifeState();
    const auto &Hit = Context.Data.Life.ReadHitState();
    if (!Life.bInitialized || State.bRagdoll)
    {
        return;
    }
    UPhysicsAsset *Asset = Context.Mesh.GetPhysicsAsset();
    if (!ensureMsgf(Asset, TEXT("角色物理表现缺少 PhysicsAsset %s"), *Context.Character.GetName()))
    {
        return;
    }
    if (Life.Phase == EBBBCharacterLifePhase::Dead)
    {
        Context.HitReaction.ClearSimulation();
        Context.PhysicalAnimation.SetStrengthMultiplyer(0.0f);
        Context.PhysicalAnimation.SetComponentTickEnabled(false);
        Context.Mesh.SetCollisionProfileName(TEXT("Ragdoll"));
        Context.Mesh.SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
        Context.Mesh.SetAllBodiesSimulatePhysics(true);
        Context.Mesh.SetAllBodiesPhysicsBlendWeight(1.0f);
        Context.Mesh.SetSimulatePhysics(true);
        Context.Mesh.WakeAllRigidBodies();
        if (Hit.Serial > 0 && !Hit.Direction.IsNearlyZero())
        {
            Context.Mesh.AddImpulseAtLocation(Hit.Direction * 700.0f, Hit.Position, Hit.Bone);
        }
        State.bRagdoll = true;
        State.HitSerial = Hit.Serial;
        return;
    }
    if (Hit.Serial == 0 || Hit.Serial <= State.HitSerial)
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
