#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/Processors/BBBCharacterEnvironmentProcessor.h"

#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/Context/BBBCharacterLifeUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Life/FBBBCharacterDamageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"

void FBBBCharacterEnvironmentProcessor::Update(FBBBCharacterLifeUpdateContext &Context) const
{
    if (Context.Data.External.ReadNetworkIdentityState().bIsMirror)
    {
        return;
    }
    auto &Domain = Context.Data.Life;
    auto &Environment = Domain.EnvironmentState;
    auto &Input = Domain.LifeInputState;
    const auto &Life = Domain.ReadLifeState();
    const auto &World = Context.Data.External.ReadWorldState();
    if (!Life.bInitialized || Life.Phase == EBBBCharacterLifePhase::Dead)
    {
        Environment.PendingDamage = 0;
        Environment.DamagePerSecond = 0;
        Input.bHasEnvironmentResult = true;
        Input.EnvironmentSpeedScale = 1;
        return;
    }
    const double Now = World.WorldTimeSeconds;
    if (Now >= Environment.NextContactAt)
    {
        Environment.NextContactAt = Now + .1;
        float Scale = 1;
        Environment.DamagePerSecond = 0;
        const auto *Capsule = Context.Character.GetCapsuleComponent();
        const auto &Physical = Context.Data.PhysicalPresentation.ReadPhysicalPresentationState();
        const FVector Foot = Physical.bRagdoll ? Physical.PelvisLocation
            : Context.Character.GetActorLocation() - FVector::UpVector * Capsule->GetScaledCapsuleHalfHeight();
        if (const auto *Mass = Context.Character.GetWorld()->GetSubsystem<UBBBMassSubsystem>())
        {
            Mass->QuerySlimeExposure(Foot, Capsule->GetScaledCapsuleRadius(), Environment.DamagePerSecond, Scale);
        }
        Input.bHasEnvironmentResult = true;
        Input.EnvironmentSpeedScale = Scale;
    }
    Environment.PendingDamage += Environment.DamagePerSecond * FMath::Clamp(World.FrameDeltaSeconds, 0.0f, .25f);
    if (Environment.PendingDamage > 0 && (Now >= Environment.NextDamageAt || Environment.DamagePerSecond <= 0))
    {
        Context.Character.SubmitInput(FBBBCharacterDamageLocalControlPacket{{FBBBHitContribution{Environment.PendingDamage, NAME_None, Context.Character.GetActorLocation(), FVector::ZeroVector}}});
        Environment.PendingDamage = 0;
        Environment.NextDamageAt = Now + .2;
    }
}
